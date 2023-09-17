
#include <linux/videodev2.h>
#include <spdlog/spdlog.h>
#include <sys/ioctl.h>

#include <cstdint>

#include "algorithm.h"
#include "v4l/v4l_device.h"
namespace {
class RpiAwb : public v4s::Algorithm {
 private:
  v4s::Device::Ptr dev_;
  float r_gain_ = 0.0f;
  float b_gain_ = 0.0f;

 public:
  RpiAwb(v4s::Device::Ptr dev) : dev_(dev){};
  void ProcessStats(v4s::Metadata& metadata) override {
    auto r_sum = metadata.Get<int64_t>("r_sum");
    auto g_sum = metadata.Get<int64_t>("g_sum");
    auto b_sum = metadata.Get<int64_t>("b_sum");
    float r_gain = (float)g_sum / (float)r_sum;
    float b_gain = (float)g_sum / (float)b_sum;
    if (std::abs(this->b_gain_ - b_gain) > 0.01 ||
        std::abs(this->r_gain_ - r_gain) > 0.01) {
      spdlog::info("Setting gains: r: {}, b: {}", r_gain, b_gain);
      std::vector<v4l2_ext_control> ctrls(2);
      ctrls[0].id = V4L2_CID_RED_BALANCE;
      ctrls[0].value = (int32_t)(r_gain * 1000);
      ctrls[1].id = V4L2_CID_BLUE_BALANCE;
      ctrls[1].value = (int32_t)(b_gain * 1000);
      v4l2_ext_controls controls{
          .which = V4L2_CTRL_WHICH_CUR_VAL,
          .count = static_cast<uint32_t>(ctrls.size()),
          .controls = ctrls.data(),
      };
      int ret = ioctl(dev_->fd(), VIDIOC_S_EXT_CTRLS, &controls);
      if (ret < 0) {
        spdlog::error("Failed to set awb: {}", strerror(errno));
      }
      this->r_gain_ = r_gain;
      this->b_gain_ = b_gain;
    }
  }
};
v4s::RegisterAlgorithm rpi_awb("rpi-awb", [](v4s::Device::Ptr dev) {
  return std::make_shared<RpiAwb>(dev);
});
}  // namespace
