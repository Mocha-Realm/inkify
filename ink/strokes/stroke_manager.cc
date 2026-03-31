#include "ink/strokes/stroke_manager.h"

namespace ink {

void StrokeManager::AddOrUpdateStroke(const CrdtStroke& stroke) {
  auto it = strokes_.find(stroke.uuid);
  if (it == strokes_.end() || it->second.lamport_timestamp < stroke.lamport_timestamp) {
    strokes_.insert_or_assign(stroke.uuid, stroke);
  }
}

void StrokeManager::DeleteStroke(const std::string& uuid, uint64_t timestamp) {
  auto it = strokes_.find(uuid);
  if (it != strokes_.end()) {
    if (it->second.lamport_timestamp < timestamp) {
      it->second.is_deleted = true;
      it->second.lamport_timestamp = timestamp;
    }
  } else {
    CrdtStroke tombstone;
    tombstone.uuid = uuid;
    tombstone.lamport_timestamp = timestamp;
    tombstone.is_deleted = true;
    strokes_.insert_or_assign(uuid, tombstone);
  }
}

std::vector<const CrdtStroke*> StrokeManager::GetActiveStrokes() const {
  std::vector<const CrdtStroke*> active_strokes;
  for (const auto& [uuid, stroke] : strokes_) {
    if (!stroke.is_deleted) {
      active_strokes.push_back(&stroke);
    }
  }
  return active_strokes;
}

const CrdtStroke* StrokeManager::GetStroke(const std::string& uuid) const {
  auto it = strokes_.find(uuid);
  if (it != strokes_.end()) {
    return &it->second;
  }
  return nullptr;
}

}  // namespace ink