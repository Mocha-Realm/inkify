#include "ink/rendering/lod_manager.h"

#include <algorithm>
#include <cmath>

namespace ink {

namespace {

// Standard epsilon values for different detail levels.
static constexpr float kHighDetailEpsilon = 0.1f;
static constexpr float kMidDetailEpsilon = 1.0f;
static constexpr float kLowDetailEpsilon = 5.0f;

}  // namespace

float LodManager::EpsilonForScale(float scale) const {
  // If scale is high (e.g. 10.0), epsilon should be small (e.g. 0.1).
  // If scale is low (e.g. 0.1), epsilon can be large (e.g. 1.0 or 5.0).
  if (scale > 5.0f) return kHighDetailEpsilon;
  if (scale > 1.0f) return kMidDetailEpsilon;
  return kLowDetailEpsilon;
}

const PartitionedMesh& LodManager::GetMeshForScale(const Stroke& stroke, float scale) {
  float target_epsilon = EpsilonForScale(scale);
  auto& levels = cache_[&stroke];

  // Try to find a level that matches or is close.
  for (const auto& level : levels) {
    if (std::abs(level.epsilon - target_epsilon) < 0.01f) {
      return level.mesh;
    }
  }

  // If not found, generate and cache it.
  // Note: Creating a temporary stroke just to generate the mesh is safe but has some overhead.
  Stroke temp(stroke.GetBrush(), stroke.GetInputs());
  (void)temp.SetBrushEpsilon(target_epsilon);
  
  levels.push_back({target_epsilon, temp.GetShape()});
  return levels.back().mesh;
}

void LodManager::Invalidate(const Stroke* stroke) {
  cache_.erase(stroke);
}

}  // namespace ink
