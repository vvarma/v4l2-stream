#include "request.h"
#include "http-server/http-server.h"
#include <memory>

namespace v4s {
namespace internal {
// Helper function to remove trailing '\r' and leading/trailing whitespace from
// a string
std::string cleanString(const std::string &str) {
  size_t endPos = str.find_last_not_of(" \t\r\n");
  if (endPos != std::string::npos) {
    size_t startPos = str.find_first_not_of(" \t\r\n");
    return str.substr(startPos, endPos - startPos + 1);
  }
  return "";
}

Req ParseReq(std::istream &input) {
  Req req;

  // Read and parse request line
  std::string requestLine;
  std::getline(input, requestLine);
  requestLine = cleanString(
      requestLine); // Clean the line from trailing '\r' and whitespace
  std::istringstream lineStream(requestLine);

  std::string methodStr, path, versionStr;
  lineStream >> methodStr >> path >> versionStr;

  if (methodStr == "GET") {
    req.method = Method::GET;
  } else if (methodStr == "POST") {
    req.method = Method::POST;
  }

  req.path = path;
  if (versionStr == "HTTP/1.0") {
    req.version = Version::HTTP_1_0;
  } else if (versionStr == "HTTP/1.1") {
    req.version = Version::HTTP_1_1;
  }

  // Read and parse headers
  std::string headerLine;
  while (std::getline(input, headerLine) && !headerLine.empty()) {
    headerLine = cleanString(
        headerLine); // Clean the line from trailing '\r' and whitespace
    size_t colonPos = headerLine.find(':');
    if (colonPos != std::string::npos) {
      std::string headerName = headerLine.substr(0, colonPos);
      std::string headerValue =
          headerLine.substr(colonPos + 1); // Skip ':' after colon
      req.headers[headerName] = cleanString(headerValue);
    }
  }

  return req;
}

RequestImpl::RequestImpl(Req req) : req_(req) {}
} // namespace internal

Request::Request(std::shared_ptr<internal::RequestImpl> pimpl)
    : pimpl_(std::move(pimpl)) {}
Request::~Request() {}

} // namespace v4s
