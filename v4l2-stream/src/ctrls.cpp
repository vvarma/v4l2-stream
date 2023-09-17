#include "ctrls.h"

#include <cstdint>
#include <memory>
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>

#include "coro/async_generator.hpp"
#include "http-server/enum.h"
#include "http-server/http-server.h"
#include "http-server/route.h"
#include "pipeline/pipeline.h"
#include "v4l/v4l_capture.h"
#include "v4l/v4l_controls.h"
using json = nlohmann::json;
using namespace nlohmann::literals;
struct PipelineDescHandler : public hs::Handler {
  PipelineDescHandler(v4s::Pipeline pipeline) : pipeline(pipeline) {}
  coro::async_generator<hs::Response> Handle(const hs::Request req) override {
    co_yield hs::StatusCode::Ok;
    json j = pipeline.GetDesc();
    std::string body = j.dump();
    hs::Headers headers = {
        {"Content-Type", "application/json"},
        {"Content-Length", fmt::format("{}", body.size())},
    };
    co_yield headers;
    co_yield std::make_shared<hs::WritableResponseBody<std::string>>(body);
  }
  v4s::Pipeline pipeline;
};

struct PipelineDescRoute : public hs::Route {
  hs::Method GetMethod() const override { return hs::Method::GET; }
  std::string GetPath() const override { return "/pipeline"; }
  hs::Handler::Ptr GetHandler() const override {
    return std::make_shared<PipelineDescHandler>(pipeline_);
  }
  PipelineDescRoute(v4s::Pipeline pipeline) : pipeline_(pipeline) {}
  v4s::Pipeline pipeline_;
};

struct GetResp {
  std::vector<v4s::IntControl> controls;
  json ToJson() const {
    json j = json::array();
    for (auto &ctrl : controls) {
      j.push_back(ctrl.ToJson());
    }
    return j;
  }
};

struct GetCtrlsHandler : public hs::Handler {
  coro::async_generator<hs::Response> Handle(const hs::Request req) override {
    auto dev_node = req.QueryParam("dev_node");
    if (!dev_node) {
      throw hs::Exception(hs::StatusCode::BadRequest, "dev_node is required");
    }
    auto device = pipeline_.GetDevice(dev_node.value());
    if (!device) {
      throw hs::Exception(hs::StatusCode::BadRequest,
                          fmt::format("device {} not found", dev_node.value()));
    }

    auto ctrls = device.value()->GetControls();
    co_yield hs::StatusCode::Ok;
    auto resp = GetResp(ctrls).ToJson().dump();
    hs::Headers headers = {
        {"Content-Type", "application/json"},
        {"Content-Length", fmt::format("{}", resp.size())},
    };
    co_yield headers;
    co_yield std::make_shared<hs::WritableResponseBody<std::string>>(resp);
  }
  GetCtrlsHandler(v4s::Pipeline pipeline) : pipeline_(pipeline) {}
  v4s::Pipeline pipeline_;
};

struct GetCtrlsRoute : public hs::Route {
  hs::Method GetMethod() const override { return hs::Method::GET; }
  std::string GetPath() const override { return "/ctrls"; }
  hs::Handler::Ptr GetHandler() const override {
    return std::make_shared<GetCtrlsHandler>(pipeline_);
  }
  GetCtrlsRoute(const v4s::Pipeline &pipeline) : pipeline_(pipeline) {}
  v4s::Pipeline pipeline_;
};

struct SetCtrlReq {
  std::string dev_node;
  struct Ctrl {
    uint32_t id;
    int64_t val;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Ctrl, id, val);
  };
  std::vector<Ctrl> controls;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(SetCtrlReq, dev_node, controls);
};

struct SetCtrlsHandler : public hs::Handler {
  coro::async_generator<hs::Response> Handle(const hs::Request req) override {
    std::string body_str = co_await req.Body();
    json body = json::parse(body_str);
    SetCtrlReq set_ctrl_req = body;
    auto dev = pipeline_.GetDevice(set_ctrl_req.dev_node);
    if (!dev) {
      throw hs::Exception(
          hs::StatusCode::BadRequest,
          fmt::format("device {} not found", set_ctrl_req.dev_node));
    }

    for (const auto &ctrl : set_ctrl_req.controls) {
      dev.value()->SetControl(ctrl.id, ctrl.val);
    }
    auto ctrls = dev.value()->GetControls();
    co_yield hs::StatusCode::Ok;
    auto resp = GetResp(ctrls).ToJson().dump();
    hs::Headers headers = {
        {"Content-Type", "application/json"},
        {"Content-Length", fmt::format("{}", resp.size())},
    };
    co_yield headers;
    co_yield std::make_shared<hs::WritableResponseBody<std::string>>(resp);
  }
  SetCtrlsHandler(v4s::Pipeline pipeline) : pipeline_(pipeline) {}
  v4s::Pipeline pipeline_;
};

struct SetCtrlsRoute : public hs::Route {
  hs::Method GetMethod() const override { return hs::Method::POST; }
  std::string GetPath() const override { return "/ctrls"; }
  hs::Handler::Ptr GetHandler() const override {
    return std::make_shared<SetCtrlsHandler>(pipeline_);
  }
  SetCtrlsRoute(const v4s::Pipeline &pipeline) : pipeline_(pipeline) {}
  v4s::Pipeline pipeline_;
};

std::vector<hs::Route::Ptr> CtrlRoutes(v4s::Pipeline pipeline) {
  return {std::make_shared<GetCtrlsRoute>(pipeline),
          std::make_shared<SetCtrlsRoute>(pipeline),
          std::make_shared<PipelineDescRoute>(pipeline)};
}
