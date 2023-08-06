#ifndef V4L2_STREAM_ENCODER_MJPEG_ENCODER_H
#define V4L2_STREAM_ENCODER_MJPEG_ENCODER_H

#include <cstdint>
#include <vector>

#include "encoder/encoder.h"
#include "v4l/v4l_frame.h"

namespace v4s {

class MJpegEncoder;

template <> struct EncoderTraits<MJpegEncoder> {
  using Item = std::vector<uint8_t>;
  constexpr static const char Codec[] = "MJPG";
};

class MJpegEncoder : public Encoder<MJpegEncoder> {
public:
  Item EncodeFrame(Frame::Ptr frame);
  std::string ContentType();
};

} // namespace v4s

#endif // !V4L2_STREAM_ENCODER_MJPEG_ENCODER_H
