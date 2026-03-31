#ifndef INK_STROKES_CRDT_STROKE_H_
#define INK_STROKES_CRDT_STROKE_H_

#include <cstdint>
#include <optional>
#include <string>

#include "ink/strokes/stroke.h"

namespace ink {

// CRDT Stroke structure extending the base Stroke with collaborative features.
struct CrdtStroke {
  std::string uuid;
  uint64_t lamport_timestamp = 0;
  std::optional<Stroke> stroke = std::nullopt;
  bool is_deleted = false;
};

}  // namespace ink

#endif  // INK_STROKES_CRDT_STROKE_H_