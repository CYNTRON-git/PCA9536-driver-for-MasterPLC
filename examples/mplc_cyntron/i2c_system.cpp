#include "i2c_system.h"
#include "core/includes.h"

#ifdef _WIN32
#include <vector>
#include <iostream>
#include <iomanip>
#define DEBUG_ERR
namespace mplc { namespace cyntron {

    int i2c_init(const char* filename, int device_addr) {
        return 1;
    }

    int i2c_send(int fd, char* buf, size_t size) {
        std::vector<char> data(buf, buf + size);
        int printed = 0;
        PRINTLN("send via i2c:\n");
        std::cout << std::hex << std::setfill('0');
        for (auto byte: data) {
            std::cout << std::setw(2) << (int)byte << " ";
            ++printed;
            if (printed == 8) {
                std::cout << std::endl;
                printed = 0;
            }
        }
        std::cout << std::dec << std::endl;
#ifdef DEBUG_ERR
        errno = 3;
        return -1;
        
#endif
        return 0;
    }

    int i2c_terminate(int fd) {
        return 0;
    }
}}  // namespace mplc::cyntron

#else
#include <stdio.h>
#include <linux/i2c-dev.h>
//#include <i2c/smbus.h>

#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
namespace mplc { namespace cyntron {

    int i2c_init(const char* filename, int device_addr) {
        int fd = open(filename, O_RDWR);
        if (fd < 0) {
            PRINTLN("Error open file %s", strerror(errno));
            return fd;
        }
        if (ioctl(fd, I2C_SLAVE, device_addr) < 0) {
            PRINTLN("Chose device error %s", strerror(errno));
            return -1;
        }
        return fd;
    }

    int i2c_send(int fd, char* buf, size_t size) {
        if (write(fd, buf, size) < 0) {
            PRINTLN("Write to device error %s", strerror(errno));
            return -1;
        }
        return 0;
    }

    int i2c_terminate(int fd) {
        close(fd);
        return 0;
    }
}}  // namespace mplc::cyntron
#endif  // _WIN32
