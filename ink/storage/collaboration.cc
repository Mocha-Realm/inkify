#include "ink/storage/collaboration.h"

#include "ink/storage/brush.h"
#include "ink/storage/stroke_input_batch.h"

namespace ink {

void EncodeCrdtStroke(const CrdtStroke& stroke, proto::CrdtStroke& proto_out) {
  proto_out.set_uuid(stroke.uuid);
  proto_out.set_lamport_timestamp(stroke.lamport_timestamp);
  proto_out.set_is_deleted(stroke.is_deleted);
  
  if (stroke.stroke.has_value()) {
    EncodeBrush(stroke.stroke->GetBrush(), *proto_out.mutable_brush());
    EncodeStrokeInputBatch(stroke.stroke->GetInputs(), *proto_out.mutable_inputs());
  }
}

absl::StatusOr<CrdtStroke> DecodeCrdtStroke(const proto::CrdtStroke& proto) {
  CrdtStroke stroke;
  stroke.uuid = proto.uuid();
  stroke.lamport_timestamp = proto.lamport_timestamp();
  stroke.is_deleted = proto.is_deleted();
  
  if (proto.has_brush() && proto.has_inputs()) {
    auto brush_or = DecodeBrush(proto.brush());
    if (!brush_or.ok()) return brush_or.status();
    
    auto inputs_or = DecodeStrokeInputBatch(proto.inputs());
    if (!inputs_or.ok()) return inputs_or.status();
    
    stroke.stroke.emplace(*brush_or, *inputs_or);
  }
  
  return stroke;
}

}  // namespace ink
