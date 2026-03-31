#include "ink/storage/collaboration.h"

#include "absl/status/status_matchers.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "ink/brush/brush.h"
#include "ink/brush/brush_family.h"
#include "ink/color/color.h"
#include "ink/strokes/input/stroke_input_batch.h"
#include "ink/strokes/stroke.h"

namespace ink {
namespace {

using ::absl_testing::IsOk;

TEST(CollaborationTest, CrdtStrokeRoundTrip) {
  Brush brush;
  StrokeInputBatch inputs;
  Stroke stroke(brush, inputs);
  
  CrdtStroke original;
  original.uuid = "test-uuid";
  original.lamport_timestamp = 12345;
  original.is_deleted = false;
  original.stroke = stroke;
  
  proto::CrdtStroke proto;
  EncodeCrdtStroke(original, proto);
  
  auto decoded_or = DecodeCrdtStroke(proto);
  EXPECT_THAT(decoded_or.status(), IsOk());
  
  const CrdtStroke& decoded = *decoded_or;
  EXPECT_EQ(decoded.uuid, original.uuid);
  EXPECT_EQ(decoded.lamport_timestamp, original.lamport_timestamp);
  EXPECT_EQ(decoded.is_deleted, original.is_deleted);
  ASSERT_TRUE(decoded.stroke.has_value());
  // Brush and inputs should match (basic verification)
  EXPECT_EQ(decoded.stroke->GetBrush().GetSize(), original.stroke->GetBrush().GetSize());
}

TEST(CollaborationTest, DeletedStrokeRoundTrip) {
  CrdtStroke original;
  original.uuid = "deleted-uuid";
  original.lamport_timestamp = 999;
  original.is_deleted = true;
  original.stroke = std::nullopt;
  
  proto::CrdtStroke proto;
  EncodeCrdtStroke(original, proto);
  
  auto decoded_or = DecodeCrdtStroke(proto);
  EXPECT_THAT(decoded_or.status(), IsOk());
  
  const CrdtStroke& decoded = *decoded_or;
  EXPECT_EQ(decoded.uuid, original.uuid);
  EXPECT_EQ(decoded.lamport_timestamp, original.lamport_timestamp);
  EXPECT_EQ(decoded.is_deleted, original.is_deleted);
  EXPECT_FALSE(decoded.stroke.has_value());
}

}  // namespace
}  // namespace ink
