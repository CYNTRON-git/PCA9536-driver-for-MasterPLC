#include "gpio_helper.h"
#include <memory>
#include "core/includes.h"
#ifndef _WIN32
#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#endif  // ! _WIN32

namespace mplc { namespace cyntron {
    constexpr const char* GPIO_PATH = "/dev/gpiochip0";
    constexpr int GPIO_LINE = 268;
#ifdef _WIN32
    struct USBResetHelper::internal {};
    void USBResetHelper::On() {}
    void USBResetHelper::Off() {}
    USBResetHelper::USBResetHelper() {}
#else
    struct USBResetHelper::internal {
        int chip_fd = -1;
        int line_fd = -1;
        ~internal() {
            if (chip_fd >= 0)
                close(chip_fd);
            if (line_fd >= 0)
                close(line_fd);
        }
    };
    USBResetHelper::USBResetHelper(): gpio_data(new internal{open(GPIO_PATH, O_RDWR), -1}) {
        if (!gpio_data || gpio_data->chip_fd < 0) {
            PRINTLN("Could not open %s", GPIO_PATH);
            return;
        }
        struct gpiohandle_request req;
        req.lineoffsets[0] = GPIO_LINE;
        req.lines = 1;
        req.flags = GPIOHANDLE_REQUEST_OUTPUT;
        memcpy(req.consumer_label, "mplc", sizeof("mplc"));
        if (ioctl(gpio_data->chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &req) < 0) {
            PRINTLN("Could not set line to output");
            return;
        }
        gpio_data->line_fd = req.fd;
    }
    void USBResetHelper::On() {
        if (on) {
            return;
        }
        if (!gpio_data || !gpio_data->line_fd || !gpio_data->chip_fd) {
            throw std::runtime_error("не удалось открыть gpio линию");
        }
        struct gpiohandle_data data;
        data.values[0] = 1;
        if (ioctl(gpio_data->line_fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
            throw std::runtime_error("не удалось отправить данные через gpio");
        }
        on = true;
    }

    void USBResetHelper::Off() {
        if (!on) {
            return;
        }
        if (!gpio_data || !gpio_data->line_fd || !gpio_data->chip_fd) {
            throw std::runtime_error("не удалось открыть gpio линию");
        }
        struct gpiohandle_data data;
        data.values[0] = 0;
        if (ioctl(gpio_data->line_fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
            throw std::runtime_error("не удалось отправить данные через gpio");
        }
        on = false;
    }
#endif  // _WIN32

    void USBResetHelper::internal_deleter::operator()(USBResetHelper::internal* ptr) {
        delete ptr;
    }
}}  // namespace mplc::cyntron
