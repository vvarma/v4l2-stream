#include "stream.h"

#include <http-server/http-server.h>
#include <spdlog/spdlog.h>
#include <sys/types.h>

#include <exception>
#include <memory>
#include <stop_token>
#include <thread>
#include <vector>

#include "encoder/mjpeg_encoder.h"
#include "http-server/enum.h"
#include "http-server/route.h"
#include "pipeline/loader.h"
#include "pipeline/pipeline.h"
#include "v4l/v4l_bridge.h"
#include "v4l/v4l_capture.h"
#include "v4l/v4l_device.h"
#include "v4l/v4l_stream.h"
const v4s::MJpegEncoder kEncoder;

struct Handler : public hs::Handler {
  hs::Generator<hs::Response> Handle(const hs::Request &req) override {
    pipeline.Prepare();
    pipelineThread =
        std::jthread([&](std::stop_token st) { pipeline.Start(st); },
                     stop_source.get_token());
    try {
      co_yield hs::StatusCode::Ok;
      hs::Headers headers = {
          {"Content-Type", kEncoder.ContentType()},
      };
      co_yield headers;
      spdlog::debug("starting loop");
      while (!IsDone()) {
        auto frame = pipeline.Next();
        co_yield std::make_shared<
            hs::WritableResponseBody<std::vector<uint8_t>>>(frame);
      }
      spdlog::info("finished streaming");
    } catch (std::exception &e) {
      spdlog::error("got an exception {}", e.what());
    }
  }

  Handler(v4s::PipelineLoader loader) : pipeline(loader.Load(kEncoder)) {}
  ~Handler() {
    stop_source.request_stop();
    pipelineThread.join();
  }
  v4s::Pipeline<v4s::MJpegEncoder> pipeline;
  std::jthread pipelineThread;
  std::stop_source stop_source;
};
struct Route : public hs::Route {
  hs::Method GetMethod() const override { return hs::Method::GET; }
  std::string GetPath() const override { return "/stream"; }
  hs::Handler::Ptr GetHandler() const override {
    return std::make_shared<Handler>(loader);
  }
  Route(v4s::PipelineLoader loader) : loader(loader) {}

  v4s::PipelineLoader loader;
};
hs::Route::Ptr StreamRoutes(v4s::PipelineLoader loader) {
  return std::make_shared<Route>(loader);
}
