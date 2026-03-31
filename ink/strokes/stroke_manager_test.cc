#include "ink/strokes/stroke_manager.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "ink/brush/brush.h"
#include "ink/strokes/stroke.h"

namespace ink {
namespace {

TEST(StrokeManagerTest, LWWUpdate) {
  StrokeManager manager;
  
  CrdtStroke s1;
  s1.uuid = "stroke-1";
  s1.lamport_timestamp = 10;
  s1.is_deleted = false;
  
  manager.AddOrUpdateStroke(s1);
  EXPECT_EQ(manager.GetActiveStrokes().size(), 1);
  
  // Update with higher timestamp
  CrdtStroke s2 = s1;
  s2.lamport_timestamp = 20;
  manager.AddOrUpdateStroke(s2);
  EXPECT_EQ(manager.GetStroke("stroke-1")->lamport_timestamp, 20);
  
  // Update with lower timestamp (should be ignored)
  CrdtStroke s3 = s1;
  s3.lamport_timestamp = 15;
  manager.AddOrUpdateStroke(s3);
  EXPECT_EQ(manager.GetStroke("stroke-1")->lamport_timestamp, 20);
}

TEST(StrokeManagerTest, Tombstone) {
  StrokeManager manager;
  
  CrdtStroke s1;
  s1.uuid = "stroke-1";
  s1.lamport_timestamp = 10;
  manager.AddOrUpdateStroke(s1);
  
  manager.DeleteStroke("stroke-1", 20);
  EXPECT_TRUE(manager.GetStroke("stroke-1")->is_deleted);
  EXPECT_EQ(manager.GetActiveStrokes().size(), 0);
  
  // Late update should be ignored by tombstone
  CrdtStroke s2 = s1;
  s2.lamport_timestamp = 15;
  manager.AddOrUpdateStroke(s2);
  EXPECT_TRUE(manager.GetStroke("stroke-1")->is_deleted);
}

}  // namespace
}  // namespace ink
