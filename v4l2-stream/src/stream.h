#ifndef V4L2_STREAM_STREAM_H
#define V4L2_STREAM_STREAM_H

#include <http-server/http-server.h>

#include "pipeline/pipeline.h"
hs::Route::Ptr StreamRoutes(v4s::Pipeline pipeline);

#endif  // !V4L2_STREAM_STREAM_H
