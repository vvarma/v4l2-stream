#ifndef V4L2_RESPONSE_WRITER_H
#define V4L2_RESPONSE_WRITER_H
#include "http-server/http-server.h"

#include <cstdint>
#include <optional>

#include <asio/ip/tcp.hpp>

namespace v4s {
namespace internal {
class ResponseWriterImpl {
public:
  void SetContentType(const std::string &content_type);
  void Write(std::vector<uint8_t> &body);
  ~ResponseWriterImpl();
  ResponseWriterImpl(asio::ip::tcp::socket &socket);

private:
  asio::ip::tcp::socket &socket_;
};

} // namespace internal

} // namespace v4s

#endif // !V4L2_RESPONSE_WRITER_H
