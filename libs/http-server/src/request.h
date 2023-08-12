#include "http-server/http-server.h"

#include <memory>
#include <optional>

#include <fmt/core.h>

namespace v4s {

namespace internal {
enum class Method { GET, POST };
enum Version { HTTP_1_0, HTTP_1_1 };
struct Req {
  Method method;
  std::string path;
  Version version;
  std::unordered_map<std::string, std::string> headers;
};
Req ParseReq(std::istream &input);
class RequestImpl {
public:
  template <typename T> std::optional<T> Query(const std::string &key) {
    return std::nullopt;
  }
  RequestImpl(Req req);

private:
  Req req_;
};

} // namespace internal

template <typename T> T Request::Query(const std::string &key) {
  return pimpl_->Query<T>(key);
}

} // namespace v4s

namespace fmt {

template <> struct formatter<v4s::internal::Method> : formatter<string_view> {
  auto format(const v4s::internal::Method &m, format_context &ctx)
      -> decltype(ctx.out()) {
    string_view name = "unknown";
    switch (m) {
    case v4s::internal::Method::GET:
      name = "GET";
      break;
    case v4s::internal::Method::POST:
      name = "POST";
      break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};
template <> struct formatter<v4s::internal::Version> : formatter<string_view> {
  auto format(const v4s::internal::Version &v, format_context &ctx) {
    string_view name = "unknown";
    switch (v) {
    case v4s::internal::Version::HTTP_1_0:
      name = "HTTP/1.0";
      break;
    case v4s::internal::Version::HTTP_1_1:
      name = "HTTP/1.1";
      break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};
template <> struct formatter<v4s::internal::Req> {
  template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  };

  template <typename FormatContext>
  auto format(const v4s::internal::Req &v, FormatContext &ctx)
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "method: {} path: {} version: {}",
                          v.method, v.path, v.version);
  }
};
} // namespace fmt
