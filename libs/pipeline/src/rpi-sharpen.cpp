
#include <linux/videodev2.h>
#include <spdlog/spdlog.h>
#include <sys/ioctl.h>

#include <cstdint>
#include <cstring>

#include "algorithm.h"
#include "metadata.h"
#define V4L2_CID_USER_BCM2835_ISP_BASE (V4L2_CID_USER_BASE + 0x10e0)
#include "decoders/bcm2835-isp.h"
#include "v4l/v4l_device.h"
namespace {
class RpiSharpen : public v4s::Algorithm {
 private:
  v4s::Device::Ptr dev_;

 public:
  RpiSharpen(v4s::Device::Ptr dev) : dev_(dev) {}
  void Prepare(v4s::Metadata&) override {
    bcm2835_isp_sharpen sharpen{
        .enabled = 1,
        .threshold = {.num = 1, .den = 1},
        .strength = {.num = 1, .den = 1},
        .limit = {.num = 1, .den = 1},
    };
    std::vector<v4l2_ext_control> ctrls = {
        v4l2_ext_control{
            .id = V4L2_CID_USER_BCM2835_ISP_SHARPEN,
            .size = sizeof(sharpen),
            .ptr = &sharpen,
        },
    };
    v4l2_ext_controls controls{
        .which = V4L2_CTRL_WHICH_CUR_VAL,
        .count = static_cast<uint32_t>(ctrls.size()),
        .controls = ctrls.data(),
    };
    int ret = ioctl(dev_->fd(), VIDIOC_S_EXT_CTRLS, &controls);
    if (ret < 0) {
      spdlog::error("Failed to set sharpen: {}", strerror(errno));
    }
  }
};

v4s::RegisterAlgorithm rpi_denoise("rpi-sharpen", [](v4s::Device::Ptr dev) {
  return std::make_shared<RpiSharpen>(dev);
});
}  // namespace
