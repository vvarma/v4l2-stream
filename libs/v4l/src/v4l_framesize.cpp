#include "v4l/v4l_framesize.h"

namespace v4s {
bool Format::operator==(const Format &other) const {
  return codec == other.codec && height == other.height && width == other.width;
}
bool Format::operator!=(const Format &other) const { return !(*this == other); }
} // namespace v4s
