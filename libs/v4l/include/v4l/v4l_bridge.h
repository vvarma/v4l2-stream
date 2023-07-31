#ifndef V4L2_STREAM_V4L_BRIDGE_H
#define V4L2_STREAM_V4L_BRIDGE_H
#include <memory>

#include "v4l/v4l_capture.h"
#include "v4l/v4l_output.h"
namespace v4s {
namespace internal {
struct BridgeBuffers;
}
class Bridge {
public:
  typedef std::shared_ptr<Bridge> Ptr;
  Bridge(CaptureDevice capture_device, OutputDevice output_device);
  bool Poll();
  void Start();
  void Stop();
  void Process();
  void ProcessWrite();
  void ProcessRead();
  int ReadFd() const;
  int WriteFd() const;
  CaptureDevice GetCaptureDevice() const;
  OutputDevice GetOutputDevice() const;
  ~Bridge();

private:
  void QueueBuffer(int idx);

  CaptureDevice capture_device_;
  OutputDevice output_device_;
  std::unique_ptr<internal::BridgeBuffers> buffers_;
};
} // namespace v4s

#endif // !V4L2_STREAM_V4L_BRIDGE_H
