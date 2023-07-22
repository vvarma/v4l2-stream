#include "http-server/http-server.h"

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/http_defs.h>
#include <pistache/net.h>
#include <pistache/router.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "request.h"
#include "responsewriter.h"
#include "route.h"

namespace P = Pistache;
namespace v4s {

namespace internal {

class StatsRoute : public Route {
public:
  std::string Path() override { return "/stats"; }

  void Handle(const Request request, ResponseWriter response) override {
    spdlog::info("got request stats");
  }
};

class ServerImpl {
  P::Http::Endpoint server;
  P::Rest::Router router;
  std::vector<RouteImpl> routes_;

public:
  ServerImpl() : server(P::Address(P::Ipv4::any(), P::Port(9481))) {
    auto opts = P::Http::Endpoint::options().threads(1);
    server.init(opts);
    RegisterRoute(std::make_shared<StatsRoute>());
  }

  void Start() {
    server.setHandler(router.handler());
    spdlog::info("Starting server at :9481");
    server.serve();
  }

  void RegisterRoute(Route::Ptr route) {
    RouteImpl routeImp(route);
    routes_.push_back(route);
    spdlog::debug("Registered route {}", route->Path());
    P::Rest::Routes::Get(
        router, route->Path(),
        [=](P::Rest::Request req,
            P::Http::ResponseWriter rw) -> P::Rest::Route::Result {
          spdlog::info("got a request");
          Request request(std::make_shared<RequestImpl>(req));
          ResponseWriter responseWriter(
              std::make_unique<ResponseWriterImpl>(std::move(rw)));
          spdlog::info("Got a request {}", route->Path());
          route->Handle(request, std::move(responseWriter));
          return P::Rest::Route::Result::Ok;
        });
  }
  void Stop() {
    spdlog::info("Shutting down server");
    server.shutdown();
  }

  ~ServerImpl() {}
};

} // namespace internal

Server::Server() : pimpl(std::make_unique<internal::ServerImpl>()) {}
Server::~Server() { pimpl->Stop(); } // namespace v4s
void Server::Start() { pimpl->Start(); }
void Server::Stop() { pimpl->Stop(); }
void Server::RegisterRoute(Route::Ptr route) {
  routes_.push_back(route);
  pimpl->RegisterRoute(route);
}

} // namespace v4s
