#pragma once
#include "bcm2835-isp.h"
#include "v4l/v4l_frame.h"

namespace v4s {
bcm2835_isp_stats BCM2835IspStatsDecode(Frame::Ptr frame);
}  // namespace v4s
