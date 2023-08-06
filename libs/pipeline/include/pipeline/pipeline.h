#ifndef V4L2_STREAM_PIPELINE_H
#define V4L2_STREAM_PIPELINE_H
#include <memory>
#include <spdlog/spdlog.h>
#include <vector>

#include "encoder/encoder.h"
#include "v4l/v4l_bridge.h"
#include "v4l/v4l_exception.h"
#include "v4l/v4l_framesize.h"
#include "v4l/v4l_stream.h"
namespace v4s {
namespace internal {
struct BridgeIO;
class PipelineImpl {
public:
  PipelineImpl(std::vector<Bridge::Ptr> bridges);
  void Start();
  ~PipelineImpl();

private:
  std::vector<std::unique_ptr<BridgeIO>> ios_;
  std::vector<Bridge::Ptr> bridges_;
};
} // namespace internal
template <typename EncType> class Pipeline {

public:
  using OutType = typename Encoder<EncType>::Item;
  OutType Next();

  Pipeline(MMapStream::Ptr sink, Encoder<EncType> encoder)
      : Pipeline({}, sink, encoder) {}

  Pipeline(std::vector<Bridge::Ptr> bridges, MMapStream::Ptr sink,
           Encoder<EncType> encoder)
      : sink_(sink), encoder_(encoder), bridges_(bridges),
        pimpl_(std::make_unique<internal::PipelineImpl>(bridges)) {}

  ~Pipeline() = default;

  // [possible] may need to reverse order
  void Prepare() {
    std::optional<Format> last_fmt = Format{
        .codec = "RGGB",
        .height = 480,
        .width = 640,
    };
    for (const auto &bridge : bridges_) {
      if (last_fmt) {
        auto fmt = last_fmt.value();
        fmt.codec = bridge->GetCaptureDevice().GetFormat().codec;
        auto updated_fmt = bridge->GetCaptureDevice().SetFormat(fmt);
        if (fmt != updated_fmt) {
          spdlog::error("Failed to set format: exp {} obs {}", fmt,
                        updated_fmt);
          throw Exception("Failed to set format");
        }
      }
      auto fmt = bridge->GetCaptureDevice().GetFormat();
      auto updated_fmt = bridge->GetOutputDevice().SetFormat(fmt);
      if (fmt != updated_fmt) {
        spdlog::error("Failed to set format: exp {} obs {}", fmt, updated_fmt);
        throw Exception("Failed to set format");
      }
      last_fmt = fmt;
    }
    if (last_fmt) {
      auto fmt = last_fmt.value();
      fmt.codec = EncoderTraits<EncType>::Codec;
      auto updated_fmt = sink_->GetDevice().SetFormat(fmt);
      if (fmt != updated_fmt) {
        spdlog::error("Failed to set format: exp {} obs {}", fmt, updated_fmt);
        throw Exception("Failed to set format");
      }
    }
    spdlog::info("Finished configuring devices");
  }

  void Start() {
    spdlog::info("Starting pipeline");
    pimpl_->Start();
  }

private:
  MMapStream::Ptr sink_;
  Encoder<EncType> encoder_;
  std::vector<Bridge::Ptr> bridges_;
  std::unique_ptr<internal::PipelineImpl> pimpl_;
};
template <typename EncType>
typename Pipeline<EncType>::OutType Pipeline<EncType>::Next() {
  return encoder_.Encode(sink_->Next());
}
} // namespace v4s

#endif // !V4L2_STREAM_PIPELINE_H
