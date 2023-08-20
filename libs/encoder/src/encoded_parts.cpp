#include "encoded_parts.h"

#include <cstddef>

#include "encoder/encoder.h"

namespace v4s {
EncodedPart::EncodedPart(std::shared_ptr<internal::EncodedPartImpl> pimpl)
    : pimpl_(std::move(pimpl)) {}
EncodedPart::~EncodedPart() = default;
const void *EncodedPart::data() const { return pimpl_->data(); }
size_t EncodedPart::size() const { return pimpl_->size(); }

namespace internal {
EncodedStringPart::EncodedStringPart(std::string str) : str_(std::move(str)) {}
const void *EncodedStringPart::data() const { return str_.data(); }
size_t EncodedStringPart::size() const { return str_.size(); }

EncodedPlanePart::EncodedPlanePart(Frame::Ptr frame, uint32_t plane)
    : frame_(std::move(frame)), plane_(plane) {}
const void *EncodedPlanePart::data() const { return frame_->Data(plane_); }
size_t EncodedPlanePart::size() const { return frame_->Size(plane_); }
}  // namespace internal
}  // namespace v4s
