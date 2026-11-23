#include "external_rtc.h"
#include <array>
#include <cerrno>
#include <charconv>
#include <fcntl.h>
#include <fstream>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdexcept>
#include <string_view>
#include <sys/ioctl.h>
#include <unistd.h>

namespace mplc::cyntron {
namespace {
namespace fs = std::filesystem;
bool validDate(const rtc_time& t) {
    if (t.tm_year < 120 || t.tm_year > 199 || t.tm_mon < 0 || t.tm_mon > 11 ||
        t.tm_hour < 0 || t.tm_hour > 23 || t.tm_min < 0 || t.tm_min > 59 ||
        t.tm_sec < 0 || t.tm_sec > 59) return false;
    constexpr std::array<int,12> days{31,28,31,30,31,30,31,31,30,31,30,31};
    int limit = days[t.tm_mon];
    if (t.tm_mon == 1 && (1900 + t.tm_year) % 4 == 0) ++limit; // Range 2020..2099.
    return t.tm_mday >= 1 && t.tm_mday <= limit;
}

int readIoctl(int fd, unsigned long op, void* request) {
    int result;
    do { result = ioctl(fd, op, request); } while (result < 0 && errno == EINTR);
    return result;
}

class Descriptor {
public:
    Descriptor(const fs::path& p, int flags) {
        do { fd = open(p.c_str(), flags | O_CLOEXEC); } while (fd < 0 && errno == EINTR);
    }
    ~Descriptor() { if (fd >= 0) close(fd); }
    Descriptor(const Descriptor&) = delete;
    Descriptor& operator=(const Descriptor&) = delete;
    int fd = -1;
};

int bcd(unsigned byte) {
    if ((byte >> 4) > 9 || (byte & 15) > 9) return -1000;
    return 10 * int(byte >> 4) + int(byte & 15);
}

bool readRegisters(int fd, __u16 address, __u8 reg, __u8* data, __u16 length) {
    i2c_msg messages[] = {{address, 0, 1, &reg}, {address, I2C_M_RD, length, data}};
    i2c_rdwr_ioctl_data transfer{messages, 2};
    return readIoctl(fd, I2C_RDWR, &transfer) == 2;
}

std::optional<rtc_time> calendar(int fd, unsigned addr, bool ds3231) {
    // I2C_SLAVE refuses an address bound to a kernel driver. Never force access.
    int selected;
    do { selected = ioctl(fd, I2C_SLAVE, static_cast<unsigned long>(addr)); }
    while (selected < 0 && errno == EINTR);
    if (selected < 0) return {};
    __u8 status = 0, data[7]{};
    const auto address = static_cast<__u16>(addr);
    // The A40i adapter returns zero bytes with a successful rc on a four-message
    // transfer. Use two transactions; all seven calendar fields still come from one burst.
    if (ds3231 && (!readRegisters(fd, address, 0x0f, &status, 1) || (status & 0x80))) return {};
    if (!readRegisters(fd, address, ds3231 ? 0x00 : 0x02, data, sizeof(data)) ||
        (!ds3231 && (data[0] & 0x80))) return {}; // PCF8563 low-voltage flag
    rtc_time t{};
    t.tm_sec = bcd(data[0] & 0x7f);
    t.tm_min = bcd(data[1] & 0x7f);
    if (ds3231 && (data[2] & 0x40)) {
        int hour = bcd(data[2] & 0x1f);
        if (hour < 1 || hour > 12) return {};
        t.tm_hour = hour % 12 + ((data[2] & 0x20) ? 12 : 0);
    } else {
        t.tm_hour = bcd(data[2] & 0x3f);
    }
    t.tm_mday = bcd(data[ds3231 ? 4 : 3] & 0x3f);
    if (ds3231 && (data[5] & 0x80)) return {}; // 2100+ exceeds the panel's calendar range.
    t.tm_mon = bcd(data[5] & 0x1f) - 1;
    t.tm_year = bcd(data[6]) + 100;
    return validDate(t) ? std::optional<rtc_time>(t) : std::nullopt;
}

std::optional<rtc_time> readChip(const fs::path& entry, unsigned wanted, bool ds3231) {
    const std::string node = entry.filename().string();
    auto split = node.find('-');
    if (split == node.npos) return {};
    unsigned bus = 0, address = 0;
    auto first = std::from_chars(node.data(), node.data() + split, bus);
    auto last = std::from_chars(node.data() + split + 1, node.data() + node.size(), address, 16);
    if (first.ec != std::errc{} || first.ptr != node.data() + split ||
        last.ec != std::errc{} || last.ptr != node.data() + node.size() || address != wanted) return {};
    std::string name;
    std::ifstream input(entry / "name");
    if (!std::getline(input, name)) return {};
    const std::string_view expected = ds3231 ? "ds3231" : "pcf8563";
    // The board DTS also emits joined compatible names such as "ds3231,d1307".
    if (name.compare(0, expected.size(), expected) != 0 ||
        (name.size() > expected.size() && std::string_view(",._-").find(name[expected.size()]) == expected.npos)) return {};
    Descriptor dev("/dev/i2c-" + std::to_string(bus), O_RDWR);
    return dev.fd < 0 ? std::nullopt : calendar(dev.fd, address, ds3231);
}
}

rtc_time readExternalRtc(const std::filesystem::path& sysfs) {
    for (const char* name : {"rtc1","rtc2","rtc3","rtc0"}) {
        std::error_code ec;
        auto real = fs::canonical(sysfs / "class/rtc" / name, ec).string();
        if ((ec && std::string_view(name) == "rtc0") ||
            real.find("1c20400.rtc") != real.npos || real.find("sun6i-rtc") != real.npos) continue;
        Descriptor rtc(fs::path("/dev") / name, O_RDONLY);
        rtc_time t{};
        if (rtc.fd >= 0 && readIoctl(rtc.fd, RTC_RD_TIME, &t) == 0 && validDate(t)) return t;
    }
    // The SA-02m BSP can register the I2C client without exposing any /dev/rtcN for it.
    const auto devices = sysfs / "bus/i2c/devices";
    for (bool ds3231 : {true, false}) {
        std::error_code ec;
        for (fs::directory_iterator i(devices, ec), end; !ec && i != end; i.increment(ec)) {
            if (auto t = readChip(i->path(), ds3231 ? 0x68 : 0x51, ds3231)) return *t;
        }
    }
    throw std::runtime_error("External RTC unavailable or invalid");
}

rtc_time ExternalRtc::read() {
    const auto now = std::chrono::steady_clock::now();
    if (now >= next_) {
        next_ = now + std::chrono::seconds(1);
        sample_.reset(); // A failed sample must invalidate the preceding good time.
        sample_ = readExternalRtc("/sys");
    }
    if (!sample_) throw std::runtime_error("External RTC unavailable or invalid");
    return *sample_;
}
}
