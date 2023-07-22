#ifndef V4L2_STREAM_PIPELINE_H
#define V4L2_STREAM_PIPELINE_H

#include "encoder/encoder.h"
#include "v4l/v4l_stream.h"
namespace v4s {
template <typename EncType> class Pipeline {

public:
  using OutType = typename Encoder<EncType>::Item;
  OutType Next();

  Pipeline(MMapStream::Ptr source, Encoder<EncType> encoder)
      : source_(source), encoder_(encoder) {}

private:
  MMapStream::Ptr source_;
  Encoder<EncType> encoder_;
};
template <typename EncType>
typename Pipeline<EncType>::OutType Pipeline<EncType>::Next() {
  return encoder_.Encode(source_->Next());
}
} // namespace v4s

#endif // !V4L2_STREAM_PIPELINE_H
