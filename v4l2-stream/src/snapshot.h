#pragma once
#include <http-server/http-server.h>

#include "pipeline/pipeline.h"

hs::Route::Ptr SnapshotRoutes(v4s::Pipeline pipeline);
