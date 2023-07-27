#include <v4l/v4l_caps.h>
namespace v4s {
bool Capabilities::IsMPlane() const { return (caps & kMultiPlaneDevice).any(); }

bool Capabilities::IsCapture() const {
  return (caps & kVideoCaptureDevice).any();
}
bool Capabilities::IsOutput() const {
  return (caps & kVideoOutputDevice).any();
}

} // namespace v4s
