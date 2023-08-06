#include "pipeline/pipeline.h"

#include <cstring>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <ev++.h>
#include <spdlog/spdlog.h>

#include "v4l/v4l_bridge.h"

namespace v4s {
namespace internal {
class BridgeIO {
public:
  explicit BridgeIO(Bridge::Ptr bridge) : bridge_(bridge) {
    // write_io_.set<BridgeIO, &BridgeIO::WriteCb>(this);
    // write_io_.start(bridge->WriteFd(), ev::WRITE);
    // read_io_.set<BridgeIO, &BridgeIO::ReadCb>(this);
    // read_io_.start(bridge->ReadFd(), ev::READ);
  }
  void Start() { bridge_->Start(); }

private:
  void ReadCb(ev::io &w, int revents) {
    spdlog::debug("got a read cb {} {}", w.fd, revents);
    bridge_->ProcessRead();
  }
  void WriteCb(ev::io &w, int revents) {
    spdlog::debug("got a write cb {} {}", w.fd, revents);
    bridge_->ProcessWrite();
  }
  Bridge::Ptr bridge_;
  ev::io read_io_, write_io_;
};

PipelineImpl::PipelineImpl(std::vector<Bridge::Ptr> bridges)
    : bridges_(bridges) {
  for (auto bridge : bridges) {
    ios_.push_back(std::make_unique<BridgeIO>(bridge));
  }
}
void PipelineImpl::Start() {
  std::vector<pollfd> fds;
  typedef std::function<void()> callback;
  std::unordered_map<int, callback> read_callbacks;
  std::unordered_map<int, callback> write_callbacks;
  std::unordered_set<int> all_fds;
  for (const auto &bridge : bridges_) {
    read_callbacks[bridge->ReadFd()] = [bridge]() { bridge->ProcessRead(); };
    write_callbacks[bridge->WriteFd()] = [bridge]() { bridge->ProcessWrite(); };
    all_fds.insert(bridge->ReadFd());
    all_fds.insert(bridge->WriteFd());
  }
  for (auto fd : all_fds) {
    pollfd pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.fd = fd;
    if (read_callbacks.count(fd)) {
      spdlog::info("attach a callback to pollin for {}", fd);
      pfd.events |= POLLIN;
    }
    if (write_callbacks.count(fd)) {
      spdlog::info("attach a callback to pollout for {}", fd);
      pfd.events |= POLLOUT;
    }
    spdlog::info("{} poll flags {}", fd, pfd.events);
    fds.push_back(pfd);
    struct v4l2_event_subscription sub;
    memset(&sub, 0, sizeof(sub));
    sub.type = V4L2_EVENT_EOS;
    int ret = ioctl(fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
    if (ret < 0) {
      spdlog::info("failed to subscribe to eos for {}", fd);
    }
    memset(&sub, 0, sizeof(sub));
    sub.type = V4L2_EVENT_SOURCE_CHANGE;
    ret = ioctl(fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
    if (ret < 0) {
      spdlog::info("failed to subscribe to source change events for {}", fd);
    }
  }
  for (auto &bridge : bridges_) {
    bridge->Start();
  }
  while (true) {
    int ready = poll(fds.data(), fds.size(), -1);
    if (ready == -1) {
      throw Exception("poll failed");
    }
    for (auto &pfd : fds) {
      if (pfd.revents & POLLIN) {
        spdlog::trace("got POLLIN for {} ", pfd.fd);
        read_callbacks[pfd.fd]();
      }
      if (pfd.revents & POLLOUT) {
        spdlog::trace("got POLLOUT for {} ", pfd.fd);
        write_callbacks[pfd.fd]();
      }
      if (pfd.revents & POLLPRI) {
        spdlog::debug("got POLLPRI on {}", pfd.fd);
        v4l2_event evt;
        memset(&evt, 0, sizeof(evt));
        int ret = ioctl(pfd.fd, VIDIOC_DQEVENT, &evt);
        if (ret < 0) {
          spdlog::info("failed to dequeue event");
        }
        if (evt.type == V4L2_EVENT_EOS) {
          spdlog::info("got eos event");
        } else if (evt.type == V4L2_EVENT_SOURCE_CHANGE) {
          spdlog::info("got source change event: changes {}",
                       evt.u.src_change.changes);
        }
      }
    }
  }
}
PipelineImpl::~PipelineImpl() {}
} // namespace internal

} // namespace v4s
