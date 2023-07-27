#ifndef V4L2_STREAM_PIPELINE_H
#define V4L2_STREAM_PIPELINE_H
#include <memory>
#include <vector>

#include "encoder/encoder.h"
#include "v4l/v4l_bridge.h"
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
      : sink_(sink), encoder_(encoder),
        pimpl_(std::make_unique<internal::PipelineImpl>(bridges)) {}
  ~Pipeline() = default;
  void Start() { pimpl_->Start(); }

private:
  MMapStream::Ptr sink_;
  Encoder<EncType> encoder_;
  std::unique_ptr<internal::PipelineImpl> pimpl_;
};
template <typename EncType>
typename Pipeline<EncType>::OutType Pipeline<EncType>::Next() {
  return encoder_.Encode(sink_->Next());
}
} // namespace v4s

#endif // !V4L2_STREAM_PIPELINE_H
