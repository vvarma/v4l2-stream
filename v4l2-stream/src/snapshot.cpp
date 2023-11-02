#include "snapshot.h"

#include <http-server/route.h>
#include <spdlog/spdlog.h>

#include <thread>

#include "encoder/mjpeg_encoder.h"
#include "pipeline/loader.h"

namespace {

struct Handler : public hs::Handler {
  coro::async_generator<hs::Response> Handle(const hs::Request req) override {
    v4s::MJpegEncoder encoder;
    pipeline.Prepare(v4s::EncoderTraits<v4s::MJpegEncoder>::Codec);
    std::jthread pipelineThread(
        [&](std::stop_token st) { pipeline.Start(st); });
    try {
      co_yield hs::StatusCode::Ok;
      for (int i = 0; i < 200; ++i) pipeline.Next();
      auto frame = pipeline.Next();
      hs::Headers headers = {
          {"Content-Type", encoder.ContentType()},
          {"Content-Length", fmt::format("{}", frame->Size(0))},
      };
      co_yield headers;
      co_yield std::make_shared<hs::WritableResponseBody<v4s::EncodedPart>>(
          encoder.EncodeFrameBody(frame));
      spdlog::info("finished streaming");
    } catch (std::exception &e) {
      spdlog::error("got an exception {}", e.what());
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
