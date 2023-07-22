#include "route.h"

#include <memory>

#include <pistache/http.h>
#include <pistache/router.h>
#include <spdlog/spdlog.h>

#include "http-server/http-server.h"
#include "request.h"
#include "responsewriter.h"

namespace P = Pistache;
namespace v4s {

namespace internal {
RouteImpl::RouteImpl(Route::Ptr route) : name(route->Path()), route_(route) {}
void RouteImpl::Handle(const P::Rest::Request &req,
                       P::Http::ResponseWriter writer) {
  spdlog::info("h1");
  Request request(std::make_shared<RequestImpl>(req));
  spdlog::info("h2");
  ResponseWriter responseWriter(
      std::make_unique<ResponseWriterImpl>(std::move(writer)));
  spdlog::info("h3");
  if (route_) {
    spdlog::info("h4 {}", route_.use_count());
    spdlog::info("h4.5 {}", name);
    spdlog::info("h5 {}", route_.get()->Path());
  }
  spdlog::info("Got a request {}", route_->Path());
  route_->Handle(request, std::move(responseWriter));
}
void RouteImpl::Register(P::Rest::Router &router) {
  spdlog::info("h0.5 {}", name);
  P::Rest::Routes::Get(router, route_->Path(),
                       P::Rest::Routes::bind(&RouteImpl::Handle, this));
}

} // namespace internal
} // namespace v4s
