#ifndef V4L2_STREAM_PIPELINE_LOADER_H
#define V4L2_STREAM_PIPELINE_LOADER_H

#include <memory>

#include "encoder/encoder.h"
#include "pipeline/config.h"
#include "pipeline/pipeline.h"
#include "v4l/v4l_capture.h"
#include "v4l/v4l_device.h"
#include "v4l/v4l_exception.h"
#include "v4l/v4l_stream.h"
namespace v4s {

class PipelineLoader {
 public:
  explicit PipelineLoader(PipelineConfig config);
  template <typename EncType>
  Pipeline<EncType> Load(Encoder<EncType> encoder) const {
    if (devices_.empty()) {
      throw Exception("No devices to load");
    }
    if (devices_.size() == 1) {
      auto cap_device = devices_[0]->TryCapture();
      if (!cap_device) {
        throw Exception("Pipeline needs a capture device");
      }
      return v4s::Pipeline<EncType>(
          std::make_shared<MMapStream>(cap_device.value()), encoder);
    }
    std::vector<Bridge::Ptr> bridges;
    for (size_t i = 0; i < devices_.size() - 1; i++) {
      auto capture_device = devices_[i]->TryCapture();
      auto output_device = devices_[i + 1]->TryOutput();
      if (!capture_device || !output_device) {
        throw Exception("Bridge needs a capture device and an output device");
      }
      bridges.emplace_back(std::make_shared<Bridge>(capture_device.value(),
                                                    output_device.value()));
    }
    auto cap_device = devices_[devices_.size() - 1]->TryCapture();
    if (!cap_device) {
      throw Exception("Pipeline needs a capture device");
    }
    v4s::Pipeline<EncType> pipeline(
        bridges, std::make_shared<MMapStream>(cap_device.value()), encoder);
    return pipeline;
  }

 private:
  std::vector<Device::Ptr> devices_;
};
}  // namespace v4s

#endif  // !#ifndef V4L2_STREAM_PIPELINE_LOADER_H
