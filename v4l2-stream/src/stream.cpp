#include "stream.h"

#include <http-server/http-server.h>
#include <spdlog/spdlog.h>
#include <sys/types.h>

#include <exception>
#include <memory>
#include <stop_token>
#include <thread>

#include "encoder/encoder.h"
#include "encoder/mjpeg_encoder.h"
#include "http-server/enum.h"
#include "http-server/route.h"
#include "pipeline/loader.h"
#include "pipeline/pipeline.h"
namespace {

struct Handler : public hs::Handler {
  coro::async_generator<hs::Response> Handle(const hs::Request req) override {
    v4s::MJpegEncoder encoder;
    pipeline.Prepare(v4s::EncoderTraits<v4s::MJpegEncoder>::Codec);
    pipelineThread =
        std::jthread([&](std::stop_token st) { pipeline.Start(st); },
                     stop_source.get_token());
    try {
      co_yield hs::StatusCode::Ok;
      hs::Headers headers = {
          {"Content-Type", encoder.ContentType()},
      };
      co_yield headers;
      spdlog::debug("starting loop");
      while (!IsDone()) {
        for (auto frame : encoder.EncodeFrame(pipeline.Next())) {
          co_yield std::make_shared<hs::WritableResponseBody<v4s::EncodedPart>>(
              frame);
        }
      }
      spdlog::info("finished streaming");
      stop_source.request_stop();
    } catch (std::exception &e) {
      spdlog::error("got an exception {}", e.what());
    }
  }

  Handler(v4s::Pipeline pipeline) : pipeline(pipeline) {}
  ~Handler() {
    stop_source.request_stop();
    if (pipelineThread.joinable()) pipelineThread.join();
  }
  v4s::Pipeline pipeline;
  std::jthread pipelineThread;
  std::stop_source stop_source;
};

struct Route : public hs::Route {
  hs::Method GetMethod() const override { return hs::Method::GET; }
  std::string GetPath() const override { return "/stream"; }
  hs::Handler::Ptr GetHandler() const override {
    return std::make_shared<Handler>(loader.Load());
  }
  Route(v4s::PipelineLoader loader) : loader(loader) {}

  v4s::PipelineLoader loader;
};
}  // namespace

hs::Route::Ptr StreamRoutes(v4s::PipelineLoader loader) {
  return std::make_shared<::Route>(loader);
}
