#ifndef V4L2_STREAM_PIPELINE_IMPL_H
#define V4L2_STREAM_PIPELINE_IMPL_H

#include <stop_token>

#include "pipeline/pipeline.h"
#include "v4l/v4l_bridge.h"
#include "v4l/v4l_device.h"
#include "v4l/v4l_exception.h"
#include "v4l/v4l_frame.h"
#include "v4l/v4l_stream.h"
namespace v4s::internal {

class PipelineImpl {
 public:
  PipelineImpl(MMapStream::Ptr sink, std::vector<Bridge::Ptr> bridges);
  void Start(std::stop_token stop_token);
  ~PipelineImpl();
  void Prepare(std::string sink_codec);
  v4s::Frame::Ptr Next();
  std::optional<Device::Ptr> GetDevice(std::string_view dev_node) const;
  PipelineDesc GetDesc() const;

 private:
  MMapStream::Ptr sink_;
  std::vector<Bridge::Ptr> bridges_;
};

}  // namespace v4s::internal
#endif  // !#ifndef V4L2_STREAM_PIPELINE_IMPL_H
