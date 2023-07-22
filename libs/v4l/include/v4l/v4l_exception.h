#ifndef V4L2_STREAM_V4L_EXCEPTION_H
#define V4L2_STREAM_V4L_EXCEPTION_H

#include <exception>
#include <string>
namespace v4s {
class Exception : public std::exception {
  std::string msg;

public:
  explicit Exception(const std::string &msg);
  const char *what() const noexcept override;
};

} // namespace v4s

#endif // !V4L2_STREAM_V4L_EXCEPTION_H
