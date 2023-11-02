#pragma once
#include <http-server/http-server.h>

#include "pipeline/loader.h"
hs::Route::Ptr StreamRoutes(v4s::PipelineLoader loader);
