#ifndef INK_RENDERING_LOD_MANAGER_H_
#define INK_RENDERING_LOD_MANAGER_H_

#include <map>
#include <vector>

#include "ink/geometry/partitioned_mesh.h"
#include "ink/strokes/stroke.h"

namespace ink {

// LodManager manages Level of Detail (LOD) for strokes by caching meshes
// generated at different epsilon (simplification) values.
class LodManager {
 public:
  struct LodLevel {
    float epsilon;
    PartitionedMesh mesh;
  };

  LodManager() = default;

  // Returns the mesh for the given stroke that best matches the requested scale.
  // 'scale' is the ratio of screen pixels to stroke-space units.
  // A higher scale (zoomed in) requires a smaller epsilon (more detail).
  const PartitionedMesh& GetMeshForScale(const Stroke& stroke, float scale);

  // Invalidates the cache for a given stroke (e.g., if the stroke is modified).
  void Invalidate(const Stroke* stroke);

 private:
  // Simple heuristic to determine the desired epsilon based on scale.
  float EpsilonForScale(float scale) const;

  std::map<const Stroke*, std::vector<LodLevel>> cache_;
};

}  // namespace ink

#endif  // INK_RENDERING_LOD_MANAGER_H_
