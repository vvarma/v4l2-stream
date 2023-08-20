#ifndef V4L2_STREAM_ENCODER_H
#define V4L2_STREAM_ENCODER_H
#include <coro/generator.hpp>
#include <cstddef>

#include "v4l/v4l_frame.h"

namespace v4s {
namespace internal {
struct EncodedPartImpl;
}

// https://stackoverflow.com/a/45801893
template <typename T>
struct EncoderTraits;

class EncodedPart {
 public:
  const void *data() const;
  size_t size() const;

  EncodedPart(std::shared_ptr<internal::EncodedPartImpl> pimpl);
  ~EncodedPart();

 private:
  std::shared_ptr<internal::EncodedPartImpl> pimpl_;
};

template <typename T>
class Encoder {
 public:
  coro::generator<EncodedPart> Encode(Frame::Ptr frame) {
    return static_cast<T *>(this)->EncodeFrame(frame);
  }
};

}  // namespace v4s

#endif  // !V4L2_STREAM_ENCODER_H
