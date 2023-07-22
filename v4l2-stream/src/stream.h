#ifndef V4L2_STREAM_STREAM_H
#define V4L2_STREAM_STREAM_H

#include "http-server/http-server.h"
#include "v4l/v4l_device.h"
v4s::Route::Ptr StreamRoutes(v4s::Device::Ptr device);

#endif // !V4L2_STREAM_STREAM_H
