#pragma once
#include <http-server/http-server.h>

#include "pipeline/pipeline.h"
hs::Route::Ptr StreamRoutes(v4s::Pipeline pipeline);
