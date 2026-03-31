#ifndef INK_STROKES_STROKE_MANAGER_H_
#define INK_STROKES_STROKE_MANAGER_H_

#include <string>
#include <vector>
#include <unordered_map>

#include "ink/strokes/crdt_stroke.h"

namespace ink {

class StrokeManager {
 public:
  StrokeManager() = default;

  // Adds a new stroke or updates an existing one based on LWW (Last Write Wins) using lamport_timestamp.
  void AddOrUpdateStroke(const CrdtStroke& stroke);

  // Marks a stroke as deleted using a tombstone, respecting LWW.
  void DeleteStroke(const std::string& uuid, uint64_t timestamp);

  // Retrieves all active (non-deleted) strokes.
  std::vector<const CrdtStroke*> GetActiveStrokes() const;

  // Retrieves a stroke by UUID. Returns nullptr if not found.
  const CrdtStroke* GetStroke(const std::string& uuid) const;

 private:
  std::unordered_map<std::string, CrdtStroke> strokes_;
};

}  // namespace ink

#endif  // INK_STROKES_STROKE_MANAGER_H_