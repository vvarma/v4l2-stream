#ifndef V4L2_STREAM_ROUTE_H
#include "http-server/http-server.h"

#include <pistache/router.h>

namespace P = Pistache;
namespace v4s {

namespace internal {
class RouteImpl {
  std::string name;
  Route::Ptr route_;
  public:
  RouteImpl(Route::Ptr route);
  void Handle(const P::Rest::Request &req, P::Http::ResponseWriter writer);
  void Register(P::Rest::Router &router);
};
} // namespace internal
} // namespace v4s

#endif // !V4L2_STREAM_ROUTE_H
