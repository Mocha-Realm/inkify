#ifndef INK_STROKES_CHUNK_H_
#define INK_STROKES_CHUNK_H_

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "absl/hash/hash.h"
#include "ink/geometry/internal/static_rtree.h"

namespace ink {

struct ChunkIndex {
  int64_t x;
  int64_t y;

  bool operator==(const ChunkIndex& other) const {
    return x == other.x && y == other.y;
  }

  template <typename H>
  friend H AbslHashValue(H h, const ChunkIndex& idx) {
    return H::combine(std::move(h), idx.x, idx.y);
  }
};

struct Chunk {
  ChunkIndex index;
  std::vector<std::string> stroke_uuids;
  std::unique_ptr<geometry_internal::StaticRTree<std::string>> spatial_index;
};

}  // namespace ink

#endif  // INK_STROKES_CHUNK_H_