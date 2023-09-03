#ifndef V4L2_STREAM_UTIL_H
#define V4L2_STREAM_UTIL_H
#include <linux/videodev2.h>

#include <cstdint>

#include "v4l/v4l_device.h"
#include "v4l/v4l_framesize.h"

uint32_t requestBuffers(int fd, v4s::BufType bufType, uint32_t num_bufs);

uint32_t getNumPlanes(int fd, v4s::BufType bufType);

std::vector<std::vector<v4l2_plane>> allocatePlanes(v4s::Device::Ptr device,
                                                    v4s::BufType buf_type,
                                                    int num_bufs);

void *allocateBuffer(int fd, int offset, size_t length);

v4s::Format getFormat(v4s::Device::Ptr device, v4s::BufType buf_type);

v4s::Format setFormat(v4s::Device::Ptr device, v4s::BufType buf_type,
                      v4s::Format format);
unsigned int ToFourcc(std::string fourcc);

#endif
