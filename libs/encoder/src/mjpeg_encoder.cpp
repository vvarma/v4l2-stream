#include "encoder/mjpeg_encoder.h"

#include <fmt/format.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <string>

#include "v4l/v4l_frame.h"
namespace v4s {

constexpr char kBoundary[] = "v4s-versionId";
constexpr char kHeader[] = "multipart/x-mixed-replace;boundary=v4s-versionId";

std::vector<uint8_t> MJpegEncoder::EncodeFrame(Frame::Ptr frame) {
  assert(frame->NumPlanes() == 1);

  std::vector<uint8_t> buf;
  std::string part = "Content-Type: image/jpeg\r\n";
  std::copy(part.begin(), part.end(), std::back_inserter(buf));

  frame->Process(0, [&buf](uint8_t *data, uint64_t len) {
    auto part = fmt::format("Content-Length: {}\r\n\r\n", len);
    std::copy(part.begin(), part.end(), std::back_inserter(buf));
    std::copy(data, data + len, std::back_inserter(buf));
    part = fmt::format("\r\n--{}\r\n", kBoundary);
    std::copy(part.begin(), part.end(), std::back_inserter(buf));
  });

  return buf;
}

std::string MJpegEncoder::ContentType() { return kHeader; }

}  // namespace v4s
