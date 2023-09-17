
#include <linux/videodev2.h>
#include <spdlog/spdlog.h>
#include <sys/ioctl.h>

#include "algorithm.h"
#include "metadata.h"
#define V4L2_CID_USER_BCM2835_ISP_BASE (V4L2_CID_USER_BASE + 0x10e0)
#include "decoders/bcm2835-isp.h"
#include "v4l/v4l_device.h"
namespace {
class RpiDenoise : public v4s::Algorithm {
 private:
  v4s::Device::Ptr dev_;

 public:
  RpiDenoise(v4s::Device::Ptr dev) : dev_(dev) {}
  void Prepare(v4s::Metadata&) override {
    bcm2835_isp_cdn denoise{
        .enabled = 1,
        .mode = CDN_MODE_FAST,
    };
    bcm2835_isp_geq geq{
        .enabled = 1,
        .offset = 204,
        .slope =
            {
                .num = 16,
                .den = 1000,
            },
    };
    std::vector<v4l2_ext_control> ctrls = {
        v4l2_ext_control{
            .id = V4L2_CID_USER_BCM2835_ISP_CDN,
            .p_u8 = reinterpret_cast<uint8_t*>(&denoise),
        },
        v4l2_ext_control{
            .id = V4L2_CID_USER_BCM2835_ISP_GEQ,
            .p_u8 = reinterpret_cast<uint8_t*>(&geq),
        },
    };
    v4l2_ext_controls controls{
        .which = V4L2_CTRL_WHICH_CUR_VAL,
        .count = 1,
        .controls = ctrls.data(),
    };
    int ret = ioctl(dev_->fd(), VIDIOC_S_EXT_CTRLS, &controls);
    if (ret < 0) {
      spdlog::error("Failed to set denoise");
    }
  }
};

v4s::RegisterAlgorithm rpi_denoise("rpi-denoise", [](v4s::Device::Ptr dev) {
  return std::make_shared<RpiDenoise>(dev);
});
}  // namespace
