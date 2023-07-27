#ifndef V4L2_STREAM_V4L_BRIDGE_H
#define V4L2_STREAM_V4L_BRIDGE_H

#include "v4l/v4l_device.h"
#include <memory>
namespace v4s {
namespace internal {
struct BridgeBuffers;
}
class Bridge {
public:
  typedef std::shared_ptr<Bridge> Ptr;
  Bridge(Device::Ptr capture_device, Device::Ptr output_device);
  bool Poll();
  void Start();
  void Stop();
  void Process();
  void ProcessWrite();
  void ProcessRead();
  int ReadFd() const;
  int WriteFd() const;
  ~Bridge();

private:
  void QueueBuffer(int idx);

  Device::Ptr capture_device_, output_device_;
  std::unique_ptr<internal::BridgeBuffers> buffers_;
};
} // namespace v4s

#endif // !V4L2_STREAM_V4L_BRIDGE_H
