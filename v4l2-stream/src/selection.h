#pragma once

#include <http-server/http-server.h>

#include "pipeline/pipeline.h"
std::vector<hs::Route::Ptr> SelectionRoutes(v4s::Pipeline pipeline);
