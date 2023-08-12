#include <spdlog/spdlog.h>

#include <exception>
#include <vector>

#include "encoder/mjpeg_encoder.h"
#include "http-server/http-server.h"
#include "http-server/response-writer.h"
#include "pipeline/loader.h"
#include "pipeline/pipeline.h"
#include "v4l/v4l_bridge.h"
#include "v4l/v4l_capture.h"
#include "v4l/v4l_device.h"
#include "v4l/v4l_stream.h"

hs::Route StreamRoutes(v4s::PipelineLoader loader) {
  auto handler = [=](hs::Request req, hs::ResponseWriter::Ptr rw) {
    v4s::MJpegEncoder encoder;
    auto pipeline = loader.Load(encoder);

    try {
      pipeline.Prepare();
      std::thread pipelineThread([&] { pipeline.Start(); });
      rw->SetContentType(encoder.ContentType());
      spdlog::debug("starting loop");
      while (true) {
        auto frame = pipeline.Next();
        spdlog::debug("got a frame");
        rw->Write(frame);
      }
    } catch (std::exception &e) {
      spdlog::error("got an exception {}", e.what());
    }
  };
  return hs::Route(hs::Method::GET, "/stream", handler);
}
