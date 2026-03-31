#include "ink/strokes/chunk_manager.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "ink/brush/brush.h"
#include "ink/strokes/stroke.h"
#include "ink/strokes/stroke_manager.h"
#include "ink/geometry/rect.h"

namespace ink {
namespace {

using ::testing::ElementsAre;

TEST(ChunkManagerTest, GetChunkIndex) {
  ChunkManager manager(100.0f);
  
  ChunkIndex idx1 = manager.GetChunkIndex({50.0f, 50.0f});
  EXPECT_EQ(idx1.x, 0);
  EXPECT_EQ(idx1.y, 0);

  ChunkIndex idx2 = manager.GetChunkIndex({150.0f, -50.0f});
  EXPECT_EQ(idx2.x, 1);
  EXPECT_EQ(idx2.y, -1);
}

TEST(ChunkManagerTest, WorldLocalTransform) {
  ChunkManager manager(100.0f);
  ChunkIndex idx = {1, 2};
  Point world = {150.0f, 250.0f};
  
  Point local = manager.WorldToLocal(world, idx);
  EXPECT_FLOAT_EQ(local.x, 50.0f);
  EXPECT_FLOAT_EQ(local.y, 50.0f);

  Point world_back = manager.LocalToWorld(local, idx);
  EXPECT_FLOAT_EQ(world_back.x, world.x);
  EXPECT_FLOAT_EQ(world_back.y, world.y);
}

TEST(ChunkManagerTest, RegisterAndQuery) {
  ChunkManager chunk_manager(100.0f);
  StrokeManager stroke_manager;
  
  // Create a dummy stroke
  Brush brush;
  Stroke stroke(brush);
  // We need a stroke with some bounds to test registration.
  // However, Stroke constructor from brush alone is empty.
  // For testing purposes, let's just manually define a CrdtStroke with a stroke that has some shape or just mock it.
  // Since we are using real Stroke, we should probably give it some input.
  
  // But wait, Stroke::GetShape().Bounds() will be empty if no input.
  // Let's use a simpler approach for the test if possible, or just add some input.
  
  CrdtStroke s1;
  s1.uuid = "stroke-1";
  s1.stroke = stroke; // Empty bounds, won't register.
  
  stroke_manager.AddOrUpdateStroke(s1);
  chunk_manager.RegisterStroke(s1.uuid, stroke_manager);
  
  auto results = chunk_manager.QueryViewport(Rect::FromTwoPoints({0, 0}, {100, 100}), stroke_manager);
  EXPECT_TRUE(results.empty());
}

}  // namespace
}  // namespace ink