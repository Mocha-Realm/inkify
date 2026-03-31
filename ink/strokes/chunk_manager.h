#ifndef INK_STROKES_CHUNK_MANAGER_H_
#define INK_STROKES_CHUNK_MANAGER_H_

#include <cmath>
#include <cstdint>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "ink/geometry/affine_transform.h"
#include "ink/geometry/internal/static_rtree.h"
#include "ink/geometry/point.h"
#include "ink/geometry/rect.h"
#include "ink/strokes/chunk.h"
#include "ink/strokes/stroke_manager.h"

namespace ink {

class ChunkManager {
 public:
  explicit ChunkManager(float chunk_size = 1024.0f) : chunk_size_(chunk_size) {}

  // Returns the index of the chunk containing the given world point.
  ChunkIndex GetChunkIndex(Point world_point) const;

  // Converts a world point to a local point within its chunk.
  Point WorldToLocal(Point world_point, ChunkIndex idx) const;

  // Converts a local point within a chunk back to a world point.
  Point LocalToWorld(Point local_point, ChunkIndex idx) const;

  // Returns the transform from world space to chunk-local space.
  AffineTransform GetWorldToLocalTransform(ChunkIndex idx) const;

  // Returns the transform from chunk-local space to world space.
  AffineTransform GetLocalToWorldTransform(ChunkIndex idx) const;

  // Registers a stroke in the appropriate chunks based on its bounding box.
  void RegisterStroke(const std::string& uuid, const StrokeManager& stroke_manager);

  // Queries all active strokes within the given world viewport.
  std::vector<const CrdtStroke*> QueryViewport(const Rect& viewport, const StrokeManager& stroke_manager) const;

 private:
  float chunk_size_;
  absl::flat_hash_map<ChunkIndex, Chunk> chunks_;
};

}  // namespace ink

#endif  // INK_STROKES_CHUNK_MANAGER_H_