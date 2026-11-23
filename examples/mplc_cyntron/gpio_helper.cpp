#include "gpio_helper.h"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <linux/gpio.h>
#include <stdexcept>
#include <system_error>
#include <sys/ioctl.h>
#include <unistd.h>

namespace mplc::cyntron {
namespace {
int gpioIoctl(int fd, unsigned long op, void* data) {
    int rc;
    do { rc = ioctl(fd, op, data); } while (rc < 0 && errno == EINTR);
    return rc;
}
}
USBResetHelper::~USBResetHelper() {
    if (line_fd >= 0) close(line_fd);
}
void USBResetHelper::set(bool enabled) {
    if (line_fd < 0) {
        int chip;
        do { chip = open("/dev/gpiochip0", O_RDONLY | O_CLOEXEC); } while (chip < 0 && errno == EINTR);
        if (chip < 0) throw std::system_error(errno, std::generic_category(), "Open USB GPIO");
        gpiohandle_request req{};
        req.lineoffsets[0] = 268;
        req.lines = 1;
        req.flags = GPIOHANDLE_REQUEST_OUTPUT;
        req.default_values[0] = enabled;
        std::strcpy(req.consumer_label, "mplc");
        int rc = gpioIoctl(chip, GPIO_GET_LINEHANDLE_IOCTL, &req);
        int err = errno;
        close(chip);
        if (rc < 0) throw std::system_error(err, std::generic_category(), "Request USB GPIO 268");
        line_fd = req.fd;
    } else if (level != enabled) {
        gpiohandle_data data{};
        data.values[0] = enabled;
        if (gpioIoctl(line_fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
            int err = errno;
            close(line_fd);
            line_fd = -1;
            throw std::system_error(err, std::generic_category(), "Set USB GPIO 268");
        }
    }
    level = enabled;
}
}
