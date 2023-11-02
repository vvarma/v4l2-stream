#include "selection.h"

#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>
#include <string>

#include "http-server/enum.h"
#include "http-server/route.h"
#include "pipeline/loader.h"
#include "pipeline/pipeline.h"
#include "query-params.h"
#include "v4l/v4l_device.h"
using json = nlohmann::json;
namespace v4s {
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(v4s::Rect, x, y, w, h);
NLOHMANN_JSON_SERIALIZE_ENUM(SelectionTarget,
                             {
                                 {SelectionTarget::Crop, "crop"},
                                 {SelectionTarget::Compose, "compose"},
                             });
}  // namespace v4s
namespace {
struct SetSelectionRequest {
  std::string dev_node;
  v4s::Role role;
  v4s::SelectionTarget target;
  v4s::Rect rect;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(SetSelectionRequest, dev_node, role, target,
                                 rect);
};

struct SetSelectionHandler : public hs::Handler {
  coro::async_generator<hs::Response> Handle(const hs::Request req) override {
    std::string body_str = co_await req.Body();
    json j = json::parse(body_str);
    SetSelectionRequest request = j;
    auto device = pipeline.GetDevice(request.dev_node);
    if (!device) {
      throw hs::Exception(hs::StatusCode::BadRequest,
                          fmt::format("device {} not found", request.dev_node));
    }
    device.value()->SetSelection(GetSimpleBufType(request.role), request.target,
                                 request.rect);

    co_yield hs::StatusCode::Ok;
    hs::Headers headers{
        {"Content-Type", "application/json"},
        {"Content-Length", "0"},
    };
    co_yield headers;
  }
  SetSelectionHandler(v4s::Pipeline pipeline) : pipeline(pipeline) {}
  v4s::Pipeline pipeline;
};

struct SetSelectionRoute : public hs::Route {
  hs::Method GetMethod() const override { return hs::Method::POST; }
  std::string GetPath() const override { return "/selection"; }
  hs::Handler::Ptr GetHandler() const override {
    return std::make_shared<SetSelectionHandler>(loader.Load());
  }
  SetSelectionRoute(v4s::PipelineLoader loader) : loader(loader) {}
  v4s::PipelineLoader loader;
};
struct GetSelectionHandler : public hs::Handler {
  coro::async_generator<hs::Response> Handle(const hs::Request req) override {
    auto device = GetDevice(req, pipeline);
    json rect =
        device->GetSelection(GetSimpleBufType(req), GetSelectionTarget(req));
    std::string body = rect.dump();

    co_yield hs::StatusCode::Ok;
    hs::Headers headers{
        {"Content-Type", "application/json"},
        {"Content-Length", fmt::format("{}", body.size())},
    };
    co_yield headers;
    co_yield std::make_shared<hs::WritableResponseBody<std::string>>(body);
  }
  GetSelectionHandler(v4s::Pipeline pipeline) : pipeline(pipeline) {}
  v4s::Pipeline pipeline;
};

struct GetSelectionRoute : public hs::Route {
  hs::Method GetMethod() const override { return hs::Method::GET; }
  std::string GetPath() const override { return "/selection"; }
  hs::Handler::Ptr GetHandler() const override {
    return std::make_shared<GetSelectionHandler>(loader.Load());
  }
  GetSelectionRoute(v4s::PipelineLoader loader) : loader(loader) {}
  v4s::PipelineLoader loader;
};

};  // namespace

std::vector<hs::Route::Ptr> SelectionRoutes(v4s::PipelineLoader loader) {
  return {
      std::make_shared<::GetSelectionRoute>(loader),
      std::make_shared<::SetSelectionRoute>(loader),
  };
}
