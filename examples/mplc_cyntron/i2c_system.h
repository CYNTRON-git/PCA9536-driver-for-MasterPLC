#pragma once
#include <stdlib.h>
namespace mplc { namespace cyntron {
    int i2c_init(const char* filename, int device_addr);

    int i2c_send(int fd, char* buf, size_t size);

    int i2c_terminate(int fd);
}}  // namespace mplc::cyntron
