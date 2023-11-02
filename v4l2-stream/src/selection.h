#pragma once

#include <http-server/http-server.h>

#include "pipeline/loader.h"

std::vector<hs::Route::Ptr> SelectionRoutes(v4s::PipelineLoader loader);
