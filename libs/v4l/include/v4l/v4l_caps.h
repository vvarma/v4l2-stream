#ifndef V4L2_STREAM_CAPS_
#define V4L2_STREAM_CAPS_
#include <bitset>
#include <cstdint>
#include <string>
namespace v4s {

enum Capability {
  CAP_VIDEO_CAPTURE = 0x1,
  CAP_VIDEO_OUTPUT = 0x2,
  CAP_VIDEO_CAPTURE_MPLANE = 0x1000,
  CAP_VIDEO_OUTPUT_MPLANE = 0x2000,
  CAP_VIDEO_M2M_MPLANE = 0x4000,
  CAP_VIDEO_M2M = 0x8000,
  CAP_META_CAPTURE = 0x00800000,
  CAP_READWRITE = 0x01000000,
  CAP_STREAMING = 0x04000000,
  CAP_META_OUTPUT = 0x08000000,
  CAP_IO_MC = 0x20000000,
};

constexpr std::bitset<32> kVideoCaptureDevice =
    (CAP_VIDEO_CAPTURE | CAP_VIDEO_CAPTURE_MPLANE | CAP_VIDEO_M2M_MPLANE |
     CAP_VIDEO_M2M);
constexpr std::bitset<32> kMultiPlaneDevice =
    (CAP_VIDEO_CAPTURE_MPLANE | CAP_VIDEO_M2M_MPLANE | CAP_VIDEO_OUTPUT_MPLANE);

struct Capabilities {
  std::string driver, card, bus_info;
  uint8_t kernel_version[3];
  std::bitset<32> caps;

  bool IsMPlane() const;
};

} // namespace v4s

#endif // !V4L2_STREAM_CAPS_
