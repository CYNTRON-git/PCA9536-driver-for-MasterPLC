#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

#ifdef _WIN32

namespace cyntron_ca02m {

constexpr uint8_t kAllOffMask = 0x0F;

inline uint8_t build_output_mask(bool blue_led, bool red_led, bool buzzer, bool dout) {
    uint8_t mask = kAllOffMask;
    if (blue_led) mask &= static_cast<uint8_t>(~(1u << 3));
    if (red_led)  mask &= static_cast<uint8_t>(~(1u << 0));
    if (buzzer)   mask &= static_cast<uint8_t>(~(1u << 2));
    if (dout)     mask &= static_cast<uint8_t>(~(1u << 1));
    return mask;
}

class Pca9536Controller {
public:
    void writeOutputs(uint8_t) {}
};

class UsbPowerController {
public:
    void setEnabled(bool) {}
};

}  // namespace cyntron_ca02m

#else

#include <cerrno>
#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <linux/gpio.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sstream>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>

namespace cyntron_ca02m {

constexpr char kI2cPath[] = "/dev/i2c-2";
constexpr int kI2cAddress = 0x41;
constexpr uint8_t kOutputRegister = 0x01;
constexpr uint8_t kConfigRegister = 0x03;
constexpr uint8_t kAllOffMask = 0x0F;
constexpr char kLockFile[] = "/run/lock/sa02m-pca9536.lock";
constexpr auto kLockWait = std::chrono::milliseconds(400);
constexpr auto kLockStep = std::chrono::milliseconds(20);
constexpr char kGpioChipPath[] = "/dev/gpiochip0";
constexpr int kUsbPowerLine = 268;

inline std::string format_errno(const std::string& prefix) {
    return prefix + ": " + std::strerror(errno);
}

inline uint8_t build_output_mask(bool blue_led, bool red_led, bool buzzer, bool dout) {
    uint8_t mask = kAllOffMask;
    if (blue_led) mask &= static_cast<uint8_t>(~(1u << 3));
    if (red_led)  mask &= static_cast<uint8_t>(~(1u << 0));
    if (buzzer)   mask &= static_cast<uint8_t>(~(1u << 2));
    if (dout)     mask &= static_cast<uint8_t>(~(1u << 1));
    return mask;
}

class Pca9536Session {
public:
    Pca9536Session() = default;
    Pca9536Session(const Pca9536Session&) = delete;
    Pca9536Session& operator=(const Pca9536Session&) = delete;

    ~Pca9536Session() {
        if (fd_ >= 0) {
            close(fd_);
        }
        if (lock_fd_ >= 0) {
            flock(lock_fd_, LOCK_UN);
            close(lock_fd_);
        }
    }

    void open() {
        lock_fd_ = ::open(kLockFile, O_RDWR | O_CREAT | O_CLOEXEC, 0666);
        if (lock_fd_ < 0) {
            throw std::runtime_error(format_errno("Не удалось открыть lock-файл PCA9536"));
        }

        const auto started = std::chrono::steady_clock::now();
        while (flock(lock_fd_, LOCK_EX | LOCK_NB) < 0) {
            if (errno != EWOULDBLOCK && errno != EAGAIN) {
                throw std::runtime_error(format_errno("Не удалось заблокировать шину PCA9536"));
            }
            if (std::chrono::steady_clock::now() - started >= kLockWait) {
                errno = EBUSY;
                throw std::runtime_error(format_errno("Шина PCA9536 занята"));
            }
            std::this_thread::sleep_for(kLockStep);
        }

        fd_ = ::open(kI2cPath, O_RDWR | O_CLOEXEC);
        if (fd_ < 0) {
            throw std::runtime_error(format_errno("Не удалось открыть /dev/i2c-2"));
        }

#ifdef I2C_TIMEOUT
        int timeout_10ms = 10;
        ioctl(fd_, I2C_TIMEOUT, timeout_10ms);
#endif
#ifdef I2C_RETRIES
        int retries = 1;
        ioctl(fd_, I2C_RETRIES, retries);
#endif

        if (ioctl(fd_, I2C_SLAVE, kI2cAddress) < 0) {
            throw std::runtime_error(format_errno("Не удалось выбрать устройство PCA9536"));
        }
    }

    uint8_t readRegister(uint8_t reg) {
        uint8_t reg_addr = reg;
        if (retryWrite(&reg_addr, 1) != 1) {
            throw std::runtime_error(format_errno("Не удалось выбрать регистр PCA9536"));
        }

        uint8_t value = 0;
        if (retryRead(&value, 1) != 1) {
            throw std::runtime_error(format_errno("Не удалось прочитать регистр PCA9536"));
        }
        return value;
    }

    void writeRegister(uint8_t reg, uint8_t value) {
        uint8_t buffer[2] = {reg, value};
        if (retryWrite(buffer, sizeof(buffer)) != static_cast<ssize_t>(sizeof(buffer))) {
            throw std::runtime_error(format_errno("Не удалось записать регистр PCA9536"));
        }
    }

    void ensureConfigured() {
        const uint8_t cfg = readRegister(kConfigRegister);
        if ((cfg & 0x0F) == 0x00) {
            return;
        }

        writeRegister(kOutputRegister, kAllOffMask);
        writeRegister(kConfigRegister, 0x00);
    }

private:
    ssize_t retryWrite(const void* data, size_t size) {
        ssize_t rc = -1;
        do {
            rc = ::write(fd_, data, size);
        } while (rc < 0 && errno == EINTR);
        return rc;
    }

    ssize_t retryRead(void* data, size_t size) {
        ssize_t rc = -1;
        do {
            rc = ::read(fd_, data, size);
        } while (rc < 0 && errno == EINTR);
        return rc;
    }

    int lock_fd_ = -1;
    int fd_ = -1;
};

class Pca9536Controller {
public:
    void writeOutputs(uint8_t mask) {
        Pca9536Session session;
        session.open();
        session.ensureConfigured();
        session.writeRegister(kOutputRegister, static_cast<uint8_t>(mask & 0x0F));
        const uint8_t verify = session.readRegister(kOutputRegister);
        if ((verify & 0x0F) != (mask & 0x0F)) {
            std::ostringstream oss;
            oss << "Не удалось верифицировать запись PCA9536: expected=0x"
                << std::hex << static_cast<int>(mask & 0x0F)
                << " actual=0x" << static_cast<int>(verify & 0x0F);
            throw std::runtime_error(oss.str());
        }
    }
};

class UsbPowerController {
public:
    UsbPowerController() {
        chip_fd_ = ::open(kGpioChipPath, O_RDONLY | O_CLOEXEC);
        if (chip_fd_ < 0) {
            throw std::runtime_error(format_errno("Не удалось открыть /dev/gpiochip0"));
        }

        std::memset(&request_, 0, sizeof(request_));
        request_.lineoffsets[0] = kUsbPowerLine;
        request_.lines = 1;
        request_.flags = GPIOHANDLE_REQUEST_OUTPUT;
        request_.default_values[0] = 1;
        std::strncpy(request_.consumer_label, "mplc_ca02m", sizeof(request_.consumer_label) - 1);

        if (ioctl(chip_fd_, GPIO_GET_LINEHANDLE_IOCTL, &request_) < 0) {
            throw std::runtime_error(format_errno("Не удалось запросить GPIO USB_POWER"));
        }

        line_fd_ = request_.fd;
    }

    UsbPowerController(const UsbPowerController&) = delete;
    UsbPowerController& operator=(const UsbPowerController&) = delete;

    ~UsbPowerController() {
        if (line_fd_ >= 0) {
            close(line_fd_);
        }
        if (chip_fd_ >= 0) {
            close(chip_fd_);
        }
    }

    void setEnabled(bool enabled) {
        gpiohandle_data data {};
        data.values[0] = enabled ? 0 : 1;
        if (ioctl(line_fd_, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
            throw std::runtime_error(format_errno("Не удалось переключить USB_POWER"));
        }
    }

private:
    gpiohandle_request request_ {};
    int chip_fd_ = -1;
    int line_fd_ = -1;
};

}  // namespace cyntron_ca02m

#endif
