#ifndef INK_STORAGE_COLLABORATION_H_
#define INK_STORAGE_COLLABORATION_H_

#include "absl/status/statusor.h"
#include "ink/storage/proto/collaboration.pb.h"
#include "ink/strokes/crdt_stroke.h"

namespace ink {

// Encodes a CrdtStroke into its protobuf representation.
void EncodeCrdtStroke(const CrdtStroke& stroke, proto::CrdtStroke& proto_out);

// Decodes a CrdtStroke from its protobuf representation.
absl::StatusOr<CrdtStroke> DecodeCrdtStroke(const proto::CrdtStroke& proto);

}  // namespace ink

#endif  // INK_STORAGE_COLLABORATION_H_
