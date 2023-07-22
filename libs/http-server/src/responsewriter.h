#ifndef V4L2_RESPONSE_WRITER_H
#define V4L2_RESPONSE_WRITER_H
#include "http-server/http-server.h"

#include <cstdint>
#include <optional>

#include <pistache/http.h>
#include <pistache/router.h>
namespace P = Pistache;
namespace v4s {
namespace internal {
class ResponseWriterImpl {
public:
  ResponseWriterImpl(P::Http::ResponseWriter writer);
  void SetContentType(const std::string &content_type);
  void Write(std::vector<uint8_t> &body);
  ~ResponseWriterImpl();

private:
  P::Http::ResponseWriter writer_;
  std::optional<P::Http::ResponseStream> stream_;
};

} // namespace internal

} // namespace v4s

#endif // !V4L2_RESPONSE_WRITER_H
