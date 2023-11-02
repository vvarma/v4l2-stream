#ifndef V4L2_STREAM_ENCODER_MJPEG_ENCODER_H
#define V4L2_STREAM_ENCODER_MJPEG_ENCODER_H

#include <coro/generator.hpp>
#include <cstdint>
#include <vector>

#include "encoder/encoder.h"
#include "v4l/v4l_frame.h"

namespace v4s {

class MJpegEncoder;

template <>
struct EncoderTraits<MJpegEncoder> {
  constexpr static const char Codec[] = "MJPG";
};

class MJpegEncoder : public Encoder<MJpegEncoder> {
 public:
  coro::generator<EncodedPart> EncodeFrame(Frame::Ptr frame);
  EncodedPart EncodeFrameBody(Frame::Ptr frame);
  std::string ContentType() const;
};

}  // namespace v4s

#endif  // !V4L2_STREAM_ENCODER_MJPEG_ENCODER_H
