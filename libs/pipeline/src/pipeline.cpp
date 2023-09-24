#include "pipeline/pipeline.h"

#include <ev++.h>
#include <linux/videodev2.h>
#include <spdlog/spdlog.h>
#include <sys/ioctl.h>
#include <sys/poll.h>

#include <cstring>
#include <stop_token>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "pipeline_impl.h"

namespace v4s {
namespace internal {
BridgeIO::BridgeIO(Bridge::Ptr bridge) : bridge_(bridge) {}
void BridgeIO::Start() { bridge_->Start(); }

PipelineImpl::PipelineImpl(MMapStream::Ptr sink,
                           std::vector<Bridge::Ptr> bridges)
    : sink_(sink), bridges_(bridges) {
  for (auto bridge : bridges) {
    ios_.push_back(std::make_unique<BridgeIO>(bridge));
  }
}
v4s::Frame::Ptr PipelineImpl::Next() { return sink_->Next(); }

Device::Ptr PipelineImpl::GetSource() const {
  if (bridges_.empty()) {
    return sink_->GetDevice().GetDevice();
  }
  return bridges_[0]->GetCaptureDevice().GetDevice();
}

void PipelineImpl::Start(std::stop_token stop_token) {
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
  while (!stop_token.stop_requested()) {
    int ready = poll(fds.data(), fds.size(), 100);
    if (ready == -1) {
      throw Exception("poll failed");
    } else if (ready == 0) {
      continue;
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
void PipelineImpl::Prepare(std::string sink_codec) {
  std::optional<Format> last_fmt = Format{
      .codec = "RGGB",
      .height = 480,
      .width = 640,
  };
  for (const auto &bridge : bridges_) {
    if (last_fmt) {
      auto fmt = last_fmt.value();
      fmt.codec = bridge->GetCaptureDevice().GetFormat().codec;
      auto updated_fmt = bridge->GetCaptureDevice().SetFormat(fmt);
      if (fmt != updated_fmt) {
        spdlog::error("Failed to set format: exp {} obs {}", fmt, updated_fmt);
        throw Exception("Failed to set format");
      }
    }
    auto fmt = bridge->GetCaptureDevice().GetFormat();
    auto updated_fmt = bridge->GetOutputDevice().SetFormat(fmt);
    if (fmt != updated_fmt) {
      spdlog::error("Failed to set format: exp {} obs {}", fmt, updated_fmt);
      throw Exception("Failed to set format");
    }
    last_fmt = fmt;
  }
  if (last_fmt) {
    auto fmt = last_fmt.value();
    fmt.codec = sink_codec;
    auto updated_fmt = sink_->GetDevice().SetFormat(fmt);
    if (fmt != updated_fmt) {
      spdlog::error("Failed to set format: exp {} obs {}", fmt, updated_fmt);
      throw Exception("Failed to set format");
    }
  }
  spdlog::info("Finished configuring devices");
}
PipelineImpl::~PipelineImpl() {
  sink_->DoStop();
  for (auto &bridge : bridges_) {
    bridge->Stop();
  }
  spdlog::info("Cleaning up pipeline");
}
}  // namespace internal

Pipeline::Pipeline(std::shared_ptr<internal::PipelineImpl> pimpl)
    : pimpl_(pimpl) {}
Pipeline::~Pipeline() {}

void Pipeline::Prepare(std::string sink_codec) { pimpl_->Prepare(sink_codec); }
void Pipeline::Start(std::stop_token stop_token) {
  spdlog::info("Starting pipeline");
  pimpl_->Start(stop_token);
}

Device::Ptr Pipeline::GetSource() const { return pimpl_->GetSource(); }

v4s::Frame::Ptr Pipeline::Next() { return pimpl_->Next(); }

}  // namespace v4s
