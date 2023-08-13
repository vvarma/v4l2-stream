#ifndef V4L2_STREAM_PIPELINE_IMPL_H
#define V4L2_STREAM_PIPELINE_IMPL_H

#include <stop_token>

#include "v4l/v4l_bridge.h"
#include "v4l/v4l_device.h"
#include "v4l/v4l_exception.h"
#include "v4l/v4l_frame.h"
#include "v4l/v4l_stream.h"
namespace v4s::internal {

class BridgeIO {
 public:
  explicit BridgeIO(Bridge::Ptr bridge);
  void Start();

 private:
  // void ReadCb(ev::io &w, int revents) {
  //   spdlog::debug("got a read cb {} {}", w.fd, revents);
  //   bridge_->ProcessRead();
  // }
  // void WriteCb(ev::io &w, int revents) {
  //   spdlog::debug("got a write cb {} {}", w.fd, revents);
  //   bridge_->ProcessWrite();
  // }
  Bridge::Ptr bridge_;
  // ev::io read_io_, write_io_;
};
class PipelineImpl {
 public:
  PipelineImpl(MMapStream::Ptr sink, std::vector<Bridge::Ptr> bridges);
  void Start(std::stop_token stop_token);
  ~PipelineImpl();
  void Prepare(std::string sink_codec);
  v4s::Frame::Ptr Next();
  Device::Ptr GetSource() const;

 private:
  MMapStream::Ptr sink_;
  std::vector<Bridge::Ptr> bridges_;
  std::vector<std::unique_ptr<BridgeIO>> ios_;
};

}  // namespace v4s::internal
#endif  // !#ifndef V4L2_STREAM_PIPELINE_IMPL_H
