#ifndef V4L2_STREAM_V4L_FRAMESIZE_H
#define V4L2_STREAM_V4L_FRAMESIZE_H
#include <cstdint>
#include <string>
#include <variant>

#include <fmt/core.h>

namespace v4s {
struct Format {
  std::string codec;
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

namespace fmt {
template <> struct formatter<v4s::Format> {
  template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  };

  template <typename FormatContext>
  auto format(const v4s::Format &v, FormatContext &ctx) -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "height: {} width: {} fourCC: {}", v.height,
                     v.width, v.codec);
  }
};

} // namespace std

#endif // !V4L2_STREAM_V4L_FRAMESIZE_H
