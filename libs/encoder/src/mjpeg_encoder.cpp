#include "encoder/mjpeg_encoder.h"

#include <fmt/format.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <string>

#include "coro/generator.hpp"
#include "encoded_parts.h"
#include "encoder/encoder.h"
#include "v4l/v4l_frame.h"
namespace v4s {

constexpr char kBoundary[] = "v4s-versionId";
constexpr char kHeader[] = "multipart/x-mixed-replace;boundary=v4s-versionId";

EncodedPart MJpegEncoder::EncodeFrameBody(Frame::Ptr frame) {
  return EncodedPart(std::make_shared<internal::EncodedPlanePart>(frame, 0));
}
coro::generator<EncodedPart> MJpegEncoder::EncodeFrame(Frame::Ptr frame) {
  assert(frame->NumPlanes() == 1);

  std::vector<uint8_t> buf;
  size_t size = frame->Size(0);
  co_yield EncodedPart(
      std::make_shared<internal::EncodedStringPart>(fmt::format(
          "Content-Type: image/jpeg\r\nContent-Length: {}\r\n\r\n", size)));
  co_yield EncodeFrameBody(frame);
  co_yield EncodedPart(std::make_shared<internal::EncodedStringPart>(
      fmt::format("\r\n--{}\r\n", kBoundary)));
}

std::string MJpegEncoder::ContentType() const { return kHeader; }

}  // namespace v4s
