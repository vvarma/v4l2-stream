#ifndef V4L2_STREAM_ROUTE_H
#include "http-server/http-server.h"

namespace v4s {

namespace internal {
class RouteImpl {
  std::string name;
  Route::Ptr route_;
  public:
  RouteImpl(Route::Ptr route);
};
} // namespace internal
} // namespace v4s

#endif // !V4L2_STREAM_ROUTE_H
