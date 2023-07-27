#include "pipeline/pipeline.h"

#include <ev++.h>
#include <vector>

#include "v4l/v4l_bridge.h"

namespace v4s {
namespace internal {
class BridgeIO {
public:
  explicit BridgeIO(Bridge::Ptr bridge) : bridge_(bridge) {
    read_io_.set<BridgeIO, &BridgeIO::ReadCb>(this);
    read_io_.start(bridge->ReadFd(), ev::READ);
    write_io_.set<BridgeIO, &BridgeIO::WriteCb>(this);
    write_io_.start(bridge->WriteFd(), ev::WRITE);
  }
  void Start() { bridge_->Start(); }

private:
  void ReadCb(ev::io &w, int revents) { bridge_->ProcessRead(); }
  void WriteCb(ev::io &w, int revents) { bridge_->ProcessWrite(); }
  Bridge::Ptr bridge_;
  ev::io read_io_, write_io_;
};

PipelineImpl::PipelineImpl(std::vector<Bridge::Ptr> bridges) {
  for (auto bridge : bridges) {
    ios_.push_back(std::make_unique<BridgeIO>(bridge));
  }
}
void PipelineImpl::Start() {
  for (auto &io : ios_) {
    io->Start();
  }
  auto event_loop = ev::get_default_loop();
  event_loop.run();
}
PipelineImpl::~PipelineImpl() {}
} // namespace internal

} // namespace v4s
