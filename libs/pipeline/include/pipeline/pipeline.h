#pragma once
#include <memory>
#include <stop_token>

#include "v4l/v4l_device.h"
#include "v4l/v4l_frame.h"
namespace v4s {
namespace internal {
class PipelineImpl;
}  // namespace internal
class Pipeline {
 public:
  v4s::Frame::Ptr Next();
  explicit Pipeline(std::shared_ptr<internal::PipelineImpl> pimpl);
  ~Pipeline();
  void Prepare(std::string sink_codec);
  void Start(std::stop_token stop_token);

  Device::Ptr GetSource() const;

 private:
  std::shared_ptr<internal::PipelineImpl> pimpl_;
};
}  // namespace v4s
