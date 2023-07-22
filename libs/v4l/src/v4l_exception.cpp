#include "v4l/v4l_exception.h"
#include <string>
namespace v4s {
Exception::Exception(const std::string &msg) : msg(msg) {}
const char *Exception::what() const noexcept { return msg.c_str(); }

} // namespace v4s
