#ifndef V4L2_STREAM_ENCODER_ENCODED_PARTS_H
#define V4L2_STREAM_ENCODER_ENCODED_PARTS_H
#include "encoder/encoder.h"
namespace v4s {
namespace internal {
struct EncodedPartImpl {
  virtual const void *data() const = 0;
  virtual size_t size() const = 0;
};
struct EncodedStringPart : public EncodedPartImpl {
  EncodedStringPart(std::string str);
  const void *data() const override;
  size_t size() const override;
  std::string str_;
};
struct EncodedPlanePart : public EncodedPartImpl {
  EncodedPlanePart(Frame::Ptr frame, uint32_t plane);
  const void *data() const override;
  size_t size() const override;
  Frame::Ptr frame_;
  uint32_t plane_;
};

}  // namespace internal
}  // namespace v4s
#endif
