#ifndef V4L2_STREAM_V4L_FRAME_H
#define V4L2_STREAM_V4L_FRAME_H

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
namespace v4s {
class Frame {
 public:
  typedef std::chrono::time_point<std::chrono::system_clock> time_point;
  typedef std::shared_ptr<Frame> Ptr;
  virtual uint64_t SeqId() const = 0;
  virtual time_point Time() const = 0;
  virtual uint32_t NumPlanes() const = 0;
  virtual const void* Data(uint32_t plane) const = 0;
  virtual size_t Size(uint32_t plane) const = 0;

  virtual ~Frame() = default;
};
}  // namespace v4s

#endif  // !V4L2_STREAM_V4L_FRAME_H
