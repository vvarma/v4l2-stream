#ifndef V4L2_STREAM_ENCODER_H
#define V4L2_STREAM_ENCODER_H
#include "v4l/v4l_frame.h"

namespace v4s {

// https://stackoverflow.com/a/45801893
template <typename T> struct EncoderTraits;

template <typename T> class Encoder {
public:
  using Item = typename EncoderTraits<T>::Item;
  Item Encode(Frame::Ptr frame) {
    return static_cast<T *>(this)->EncodeFrame(frame);
  }
};

} // namespace v4s

#endif // !V4L2_STREAM_ENCODER_H
