#ifndef V4L2_STREAM_PIPELINE_H
#define V4L2_STREAM_PIPELINE_H
#include <vector>

#include "encoder/encoder.h"
#include "v4l/v4l_bridge.h"
#include "v4l/v4l_stream.h"
namespace v4s {
template <typename EncType> class Pipeline {

public:
  using OutType = typename Encoder<EncType>::Item;
  OutType Next();

  Pipeline(std::vector<Bridge> bridges, MMapStream::Ptr sink,
           Encoder<EncType> encoder)
      : bridges_(bridges), sink_(sink), encoder_(encoder) {}

private:
  std::vector<Bridge> bridges_;
  MMapStream::Ptr sink_;
  Encoder<EncType> encoder_;
};
template <typename EncType>
typename Pipeline<EncType>::OutType Pipeline<EncType>::Next() {
  return encoder_.Encode(sink_->Next());
}
} // namespace v4s

#endif // !V4L2_STREAM_PIPELINE_H
