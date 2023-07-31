#ifndef V4L2_STREAM_V4L_FRAMESIZE_H
#define V4L2_STREAM_V4L_FRAMESIZE_H
#include <cstdint>
#include <variant>

#include "v4l_codec.h"
namespace v4s {
struct Format {
  Codec codec;
  uint32_t height, width;
  bool operator==(const Format &other) const;
  bool operator!=(const Format &other) const; 
};

struct DiscreteFrameSize {
  uint32_t height, width;
};
struct StepwiseFrameSize {
  uint32_t min_width, max_width, step_width, min_height, max_height,
      step_height;
};

typedef std::variant<DiscreteFrameSize, StepwiseFrameSize> FrameSize;
} // namespace v4s

#endif // !V4L2_STREAM_V4L_FRAMESIZE_H
