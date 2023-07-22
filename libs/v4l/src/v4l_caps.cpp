#include <v4l/v4l_caps.h>
namespace v4s {
bool Capabilities::IsMPlane() const { return (caps & kMultiPlaneDevice).any(); }

} // namespace v4s
