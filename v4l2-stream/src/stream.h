#ifndef V4L2_STREAM_STREAM_H
#define V4L2_STREAM_STREAM_H

#include "http-server/http-server.h"
#include "v4l/v4l_bridge.h"
#include "v4l/v4l_device.h"
#include "v4l/v4l_stream.h"
v4s::Route::Ptr StreamRoutes(std::vector<v4s::Bridge::Ptr> bridges,
                             v4s::MMapStream::Ptr stream);

#endif // !V4L2_STREAM_STREAM_H
