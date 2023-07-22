#include "http-server/http-server.h"

#include <memory>
#include <optional>

#include <pistache/router.h>

namespace P = Pistache;

namespace v4s {

namespace internal {
class RequestImpl {
public:
  template <typename T> std::optional<T> Query(const std::string &key) {
    auto q = req.query().get(key);
    if (q.isEmpty()) {
      return std::nullopt;
    }
    std::stringstream ss;
    ss << q.get();
    T res;
    ss >> res;
    return res;
  }
  RequestImpl(const P::Rest::Request &req);

private:
  P::Rest::Request req;
};
} // namespace internal

template <typename T> T Request::Query(const std::string &key) {
  return pimpl_->Query<T>(key);
}

} // namespace v4s
