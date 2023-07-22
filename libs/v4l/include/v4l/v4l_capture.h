#ifndef V4L2_STREAM_V4L_CAPTURE_H
#define V4L2_STREAM_V4L_CAPTURE_H
#include "v4l/v4l_device.h"
#include "v4l/v4l_stream.h"

namespace v4s {
class CaptureDevice {
public:
  CaptureDevice(std::shared_ptr<Device> device);

  MMapStream::Ptr Stream();

private:
  std::shared_ptr<Device> device_;
};
} // namespace v4s

#endif // !V4L2_STREAM_V4L_CAPTURE_H
