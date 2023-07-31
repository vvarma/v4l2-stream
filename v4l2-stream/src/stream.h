#ifndef V4L2_STREAM_STREAM_H
#define V4L2_STREAM_STREAM_H

#include "http-server/http-server.h"
#include "pipeline/loader.h"
v4s::Route::Ptr StreamRoutes(v4s::PipelineLoader loader);

#endif // !V4L2_STREAM_STREAM_H
