#include "route.h"

#include <memory>

#include <spdlog/spdlog.h>

#include "http-server/http-server.h"
#include "request.h"
#include "responsewriter.h"

namespace v4s {

namespace internal {
RouteImpl::RouteImpl(Route::Ptr route) : name(route->Path()), route_(route) {}

} // namespace internal
} // namespace v4s
