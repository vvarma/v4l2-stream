#include "metrics.h"

#include <chrono>
#include <coro/single_consumer_event.hpp>
#include <future>
#include <mutex>
#include <thread>

#include "http-server/enum.h"
#include "http-server/route.h"
#include "metrics/metrics.h"
using namespace std::chrono_literals;
namespace {
class Handler : public hs::Handler {
  coro::async_generator<hs::Response> Handle(const hs::Request req) {
    auto &metrics = v4s::Metrics::GetInstance();
    co_yield hs::StatusCode::Ok;
    hs::Headers headers{
        {"Content-Type", "text/event-stream"},
    };
    co_yield headers;

    for (;;) {
      nlohmann::json j = metrics.Report();
      co_yield std::make_shared<hs::WritableResponseBody<std::string>>(
          "data: " + j.dump() + "\n");
      co_yield std::make_shared<hs::WritableResponseBody<std::string>>(
          "event: metrics\n");
      coro::single_consumer_event event;
      std::thread t([&]() {
        std::this_thread::sleep_for(1s);
        event.set();
      });
      t.detach();
      co_await event;
    }
  }
};

hs::Handler::Ptr GetMetricsHandler() { return std::make_shared<Handler>(); };

ROUTE(Route, hs::Method::GET, "/metrics", GetMetricsHandler);
}  // namespace

hs::Route::Ptr MetricsRoute() { return std::make_shared<::Route>(); }
