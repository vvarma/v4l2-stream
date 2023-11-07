#include "snapshot.h"

#include <http-server/route.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <thread>

#include "encoder/mjpeg_encoder.h"
#include "http-server/enum.h"
#include "pipeline/loader.h"
#include "v4l/v4l_exception.h"
#include "v4l/v4l_frame.h"

namespace {

struct Handler : public hs::Handler {
  coro::async_generator<hs::Response> Handle(const hs::Request req) override {
    v4s::MJpegEncoder encoder;
    pipeline.Prepare(v4s::EncoderTraits<v4s::MJpegEncoder>::Codec);
    std::jthread pipelineThread([&](std::stop_token st) {
      try {
        pipeline.Start(st);
      } catch (const v4s::Exception &e) {
        spdlog::error("got an exception in start pipeline: {}", e.what());
      }
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    v4s::Frame::Ptr frame = nullptr;
    try {
      for (int i = 0; i < 20; ++i) pipeline.Next();
      frame = pipeline.Next();
    } catch (std::exception &e) {
      spdlog::error("got an exception {}", e.what());
    }
    if (frame) {
      co_yield hs::StatusCode::Ok;
      hs::Headers headers = {
          {"Content-Type", "image/jpeg"},
          {"Content-Length", fmt::format("{}", frame->Size(0))},
      };
      co_yield headers;
      co_yield std::make_shared<hs::WritableResponseBody<v4s::EncodedPart>>(
          encoder.EncodeFrameBody(frame));
      spdlog::info("finished streaming");
    } else {
      co_yield hs::StatusCode::InternalServerError;
      hs::Headers headers = {
          {"Content-Length", "0"},
      };
      co_yield headers;
    }
  }

  Handler(v4s::Pipeline pipeline) : pipeline(pipeline) {}
  ~Handler() {}
  v4s::Pipeline pipeline;
};

struct Route : public hs::Route {
  hs::Method GetMethod() const override { return hs::Method::GET; }
  std::string GetPath() const override { return "/snapshot"; }
  hs::Handler::Ptr GetHandler() const override {
    return std::make_shared<Handler>(loader.Load());
  }
  Route(v4s::PipelineLoader loader) : loader(loader) {}

  v4s::PipelineLoader loader;
};

}  // namespace

hs::Route::Ptr SnapshotRoutes(v4s::PipelineLoader loader) {
  return std::make_shared<::Route>(loader);
}
