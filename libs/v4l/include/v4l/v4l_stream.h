#ifndef V4L2_STREAM_V4L_STREAM
#define V4L2_STREAM_V4L_STREAM

#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "v4l/v4l_device.h"
#include "v4l/v4l_frame.h"
namespace v4s {

template <typename T> class Stream : public std::enable_shared_from_this<T> {

  std::atomic_bool started_;

protected:
  Device::Ptr device_;

public:
  Stream(Device::Ptr device) : device_(device), started_(false) {}
  void Start() {
    (static_cast<T *>(this))->DoStart();
    started_ = true;
  }
  void Stop() {
    if (started_)
      (static_cast<T *>(this))->DoStop();
    started_ = false;
  }

  Frame::Ptr Next() {
    if (!started_)
      Start();
    return (static_cast<T *>(this))->FetchNext();
  }
};

namespace internal {
struct MMapBuffers;
}

class MMapStream : public Stream<MMapStream> {
  std::unique_ptr<internal::MMapBuffers> buffers;

public:
  typedef std::shared_ptr<MMapStream> Ptr;
  void QueueBuffer(int idx);
  MMapStream(Device::Ptr device);
  bool Poll();
  void DoStart();
  void DoStop();
  Frame::Ptr FetchNext();
  ~MMapStream();
};

} // namespace v4s

#endif // !V4L2_STREAM_V4L_STREAM
