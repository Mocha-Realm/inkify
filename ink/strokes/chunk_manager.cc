#include "ink/strokes/chunk_manager.h"

#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace ink {

ChunkIndex ChunkManager::GetChunkIndex(Point world_point) const {
  return {static_cast<int64_t>(std::floor(world_point.x / chunk_size_)),
          static_cast<int64_t>(std::floor(world_point.y / chunk_size_))};
}

Point ChunkManager::WorldToLocal(Point world_point, ChunkIndex idx) const {
  return {world_point.x - static_cast<float>(idx.x) * chunk_size_,
          world_point.y - static_cast<float>(idx.y) * chunk_size_};
}

Point ChunkManager::LocalToWorld(Point local_point, ChunkIndex idx) const {
  return {local_point.x + static_cast<float>(idx.x) * chunk_size_,
          local_point.y + static_cast<float>(idx.y) * chunk_size_};
}

AffineTransform ChunkManager::GetWorldToLocalTransform(ChunkIndex idx) const {
  return AffineTransform::Translate(Vec{-static_cast<float>(idx.x) * chunk_size_,
                                        -static_cast<float>(idx.y) * chunk_size_});
}

AffineTransform ChunkManager::GetLocalToWorldTransform(ChunkIndex idx) const {
  return AffineTransform::Translate(Vec{static_cast<float>(idx.x) * chunk_size_,
                                        static_cast<float>(idx.y) * chunk_size_});
}

void ChunkManager::RegisterStroke(const std::string& uuid, const StrokeManager& stroke_manager) {
  const CrdtStroke* crdt_stroke = stroke_manager.GetStroke(uuid);
  if (!crdt_stroke || !crdt_stroke->stroke.has_value()) return;

  Envelope envelope = crdt_stroke->stroke->GetShape().Bounds();
  if (envelope.IsEmpty()) return;

  Rect rect = *envelope.AsRect();
  ChunkIndex min_idx = GetChunkIndex({rect.XMin(), rect.YMin()});
  ChunkIndex max_idx = GetChunkIndex({rect.XMax(), rect.YMax()});

  for (int64_t x = min_idx.x; x <= max_idx.x; ++x) {
    for (int64_t y = min_idx.y; y <= max_idx.y; ++y) {
      ChunkIndex idx = {x, y};
      auto& chunk = chunks_[idx];
      chunk.index = idx;
      chunk.stroke_uuids.push_back(uuid);
      // Invalidate the spatial index when adding new strokes.
      chunk.spatial_index.reset();
    }
  }
}

std::vector<const CrdtStroke*> ChunkManager::QueryViewport(const Rect& viewport, const StrokeManager& stroke_manager) const {
  ChunkIndex min_idx = GetChunkIndex({viewport.XMin(), viewport.YMin()});
  ChunkIndex max_idx = GetChunkIndex({viewport.XMax(), viewport.YMax()});

  std::unordered_set<std::string> seen_uuids;
  std::vector<const CrdtStroke*> results;

  for (int64_t x = min_idx.x; x <= max_idx.x; ++x) {
    for (int64_t y = min_idx.y; y <= max_idx.y; ++y) {
      auto it = chunks_.find({x, y});
      if (it == chunks_.end()) continue;

      auto& chunk = const_cast<Chunk&>(it->second);
      if (!chunk.spatial_index && !chunk.stroke_uuids.empty()) {
        chunk.spatial_index = std::make_unique<geometry_internal::StaticRTree<std::string>>(
            chunk.stroke_uuids,
            [&stroke_manager](const std::string& uuid) {
              const CrdtStroke* s = stroke_manager.GetStroke(uuid);
              if (s && s->stroke.has_value()) {
                Envelope envelope = s->stroke->GetShape().Bounds();
                if (!envelope.IsEmpty()) return *envelope.AsRect();
              }
              return Rect();
            });
      }

      if (chunk.spatial_index) {
        chunk.spatial_index->VisitIntersectedElements(
            viewport, [&seen_uuids, &results, &stroke_manager](const std::string& uuid) {
              if (seen_uuids.insert(uuid).second) {
                const CrdtStroke* s = stroke_manager.GetStroke(uuid);
                if (s && !s->is_deleted) results.push_back(s);
              }
              return true;
            });
      }
    }
  }
  return results;
}

}  // namespace ink