#include "http-server/http-server.h"

#include <istream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <asio.hpp>
#include <asio/awaitable.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/read_until.hpp>
#include <asio/use_awaitable.hpp>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include "request.h"
#include "responsewriter.h"
#include "route.h"

using asio::awaitable;
using asio::use_awaitable;
using asio::ip::tcp;
namespace this_coro = asio::this_coro;
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
  std::vector<Route::Ptr> routes_;

  awaitable<void> HandleReq(tcp::socket &socket, Req req) {
    spdlog::info("{}", req);
    for (auto &route : routes_) {
      if (route->Path() == req.path) {
        Request request(std::make_shared<RequestImpl>(req));
        ResponseWriter writer(std::make_shared<ResponseWriterImpl>(socket));
        route->Handle(request, writer);
        co_return;
      }
    }
    auto resp = fmt::format("HTTP/1.1 404 Not Found\r\n\r\n");
    co_await socket.async_write_some(asio::buffer(resp), use_awaitable);
  }
  awaitable<void> Handle(tcp::socket socket) {
    for (;;) {
      asio::streambuf buffer;
      co_await async_read_until(socket, buffer, "\r\n", use_awaitable);
      std::istream is(&buffer);
      Req req = ParseReq(is);
      co_await HandleReq(socket, req);

      break;
    }
  }

public:
  awaitable<void> Serve() {
    auto executor = co_await this_coro::executor;
    tcp::acceptor acceptor(executor, {tcp::v4(), 5555});
    for (;;) {
      tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
      co_spawn(executor, Handle(std::move(socket)), asio::detached);
    }
  }

  void RegisterRoute(Route::Ptr route) { routes_.push_back(route); }
  void Stop() {}

  ~ServerImpl() {}
};

} // namespace internal

Server::Server() : pimpl(std::make_unique<internal::ServerImpl>()) {}
Server::~Server() { pimpl->Stop(); } // namespace v4s
awaitable<void> Server::Start() {
  auto executor = co_await this_coro::executor;

  co_spawn(executor, pimpl->Serve(), asio::detached);
}
void Server::RegisterRoute(Route::Ptr route) { pimpl->RegisterRoute(route); }

} // namespace v4s
