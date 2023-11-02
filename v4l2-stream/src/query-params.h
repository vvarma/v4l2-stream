#pragma once

#include <string_view>

#include "http-server/http-server.h"
#include "http-server/request.h"
#include "pipeline/pipeline.h"
#include "v4l/v4l_device.h"
inline v4s::Device::Ptr GetDevice(const hs::Request& req,
                                  const v4s::Pipeline& pipeline) {
  auto dev_node = req.QueryParam("dev_node");
  if (!dev_node) {
    throw hs::Exception(hs::StatusCode::BadRequest,
                        "dev_node is required");
  }
  auto device = pipeline.GetDevice(dev_node.value());
  if (!device) {
    throw hs::Exception(hs::StatusCode::BadRequest,
                        fmt::format("device {} not found", dev_node.value()));
  }
  return device.value();
}
inline v4s::BufType GetSimpleBufType(v4s::Role role) {
  v4s::BufType buf_type;
  if (role == v4s::Role::SOURCE) {
    buf_type = v4s::BufType::BUF_VIDEO_CAPTURE;
  } else if (role == v4s::Role::SINK) {
    buf_type = v4s::BufType::BUF_VIDEO_OUTPUT;
  } else {
    throw hs::Exception(hs::StatusCode::BadRequest,
                        fmt::format("role not supported"));
  }
  return buf_type;
}
inline v4s::BufType GetSimpleBufType(const hs::Request& req) {
  auto role = req.QueryParam("role");
  if (!role) {
    throw hs::Exception(hs::StatusCode::BadRequest,
                        "role is required");
  }
  v4s::BufType buf_type;
  if (role == "source") {
    buf_type = v4s::BufType::BUF_VIDEO_CAPTURE;
  } else if (role == "sink") {
    buf_type = v4s::BufType::BUF_VIDEO_OUTPUT;
  } else {
    throw hs::Exception(hs::StatusCode::BadRequest,
                        fmt::format("role {} not supported", role.value()));
  }
  return buf_type;
}
inline v4s::SelectionTarget GetSelectionTarget(const hs::Request& req) {
  auto target = req.QueryParam("target");
  if (!target) {
    throw hs::Exception(hs::StatusCode::BadRequest, "target is required");
  }
  if (target == "crop") {
    return v4s::SelectionTarget::Crop;
  } else if (target == "compose") {
    return v4s::SelectionTarget::Compose;
  } else {
    throw hs::Exception(hs::StatusCode::BadRequest,
                        fmt::format("target {} not supported", target.value()));
  }
}
