#ifndef V4L2_STREAM_CTRLS_H
#define V4L2_STREAM_CTRLS_H
#include <http-server/http-server.h>

#include <vector>

#include "pipeline/loader.h"
std::vector<hs::Route::Ptr> CtrlRoutes(v4s::PipelineLoader loader);
#endif  // !#ifndef V4L2_STREAM_CTRLS_H
