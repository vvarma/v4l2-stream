#include <linux/v4l2-controls.h>
#include <linux/videodev2.h>
#include <spdlog/spdlog.h>

#include <cstddef>
#include <cstdint>
#include <vector>

#include "algorithm.h"
#include "decoders/bcm2835-isp.h"
#include "metadata.h"
#include "v4l/v4l_controls.h"
#include "v4l/v4l_device.h"

constexpr int kPipelineWidth = 13;
constexpr int kAgcRegions = 15;
constexpr int kAgcZoneWeights = 15;
constexpr int kNormFactorPow2 = 16;
constexpr int scale = kNormFactorPow2 - kPipelineWidth;

size_t medianOfHistogram(std::vector<int> hist) {
  int sum = 0;
  for (int v : hist) {
    sum += v;
  }
  int median = sum / 2;
  sum = 0;
  for (size_t i = 0; i < hist.size(); ++i) {
    sum += hist[i];
    if (sum >= median) {
      return i;
    }
  }
  return 0;
}

namespace {
class RpiAgc : public v4s::Algorithm {
 private:
  v4s::Device::Ptr dev_;

 public:
  RpiAgc(v4s::Device::Ptr dev) : dev_(dev){};
  ~RpiAgc() override = default;
  void UpdateExposureAndGain(int delta) {
    auto exposure_o = dev_->GetControl(V4L2_CID_EXPOSURE);
    auto gain_o = dev_->GetControl(V4L2_CID_ANALOGUE_GAIN);

    if (!exposure_o || !gain_o) {
      spdlog::error("Failed to get exposure or gain control");
      return;
    }
    auto exposure = exposure_o.value();
    auto gain = gain_o.value();
    if (gain.max > gain.curr + delta && gain.curr + delta > gain.min) {
      gain.SetControl(dev_->fd(), gain.curr + delta);
    } else if (exposure.max > exposure.curr + delta &&
               exposure.curr + delta > exposure.min) {
      exposure.SetControl(dev_->fd(), exposure.curr + delta);
    } else {
      spdlog::info("Exposure and gain are at their limits");
    }
  }
  void ProcessStats(v4s::Metadata& metadata) override {
    auto hist = metadata.Get<std::vector<int>>("rpi.g_hist");
    size_t median = medianOfHistogram(hist);
    if (median < hist.size() / 2 - 5) {
      UpdateExposureAndGain(5);
    } else if (median > hist.size() / 2 + 5) {
      UpdateExposureAndGain(-5);
    }
  }
};

v4s::RegisterAlgorithm reg("rpi-agc", [](v4s::Device::Ptr dev) {
  return std::make_shared<RpiAgc>(dev);
});

}  // namespace
