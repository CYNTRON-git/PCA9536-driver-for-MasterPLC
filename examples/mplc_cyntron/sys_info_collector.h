#pragma once
#include <cstdint>
#include <cmath>
#include <cerrno>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <linux/rtc.h>
#include <unistd.h>

namespace mplc::cyntron {
constexpr auto usb_mountpoint = "/media/usb";
constexpr auto sd_mountpoint = "/media/sdcard";
struct MediaData {
    bool mounted{};
    float size{};
    float used{};
    int16_t percent{};
};

inline MediaData filesystemData(const char* path) {
    struct statvfs fs{};
    if (statvfs(path, &fs) < 0 || !fs.f_blocks || !fs.f_frsize || fs.f_bavail > fs.f_blocks)
        throw std::runtime_error(std::string("Invalid filesystem statistics: ") + path);
    const double gib = fs.f_frsize / 1073741824.0;
    const auto used = fs.f_blocks - fs.f_bavail;
    return {true, static_cast<float>(fs.f_blocks * gib), static_cast<float>(used * gib),
            static_cast<int16_t>(std::round(100.0 * used / fs.f_blocks))};
}

struct ReusableSysInfo {
    std::optional<MediaData> usb, sd;
    const char* modem_path = "/sys/class/net/usb0";
    const MediaData& media(const char* path, std::optional<MediaData>& cached) {
        if (cached) return *cached;
        std::ifstream mounts("/proc/mounts");
        if (!mounts) throw std::runtime_error("Cannot open /proc/mounts");
        std::string line;
        while (std::getline(mounts, line)) {
            std::istringstream fields(line);
            std::string device, mount;
            if (!(fields >> device >> mount)) throw std::runtime_error("Invalid /proc/mounts");
            if (mount == path) {
                cached = filesystemData(path);
                return *cached;
            }
        }
        if (mounts.bad()) throw std::runtime_error("Cannot read /proc/mounts");
        cached = MediaData{};
        return *cached;
    }
    const MediaData& usbFlash() { return media(usb_mountpoint, usb); }
    const MediaData& sdCard() { return media(sd_mountpoint, sd); }
};

inline int16_t get_cpu_temp() {
    std::ifstream input("/sys/class/thermal/thermal_zone0/temp");
    long millidegrees;
    if (!(input >> millidegrees) || millidegrees < -32768000 || millidegrees > 32767000)
        throw std::runtime_error("Invalid CPU temperature");
    return static_cast<int16_t>(std::round(millidegrees / 1000.0));
}

inline int16_t get_ram_percent() {
    std::ifstream input("/proc/meminfo");
    long long total = -1, available = -1;
    std::string line;
    while (std::getline(input, line)) {
        std::istringstream fields(line);
        std::string key;
        fields >> key;
        if (key == "MemTotal:") fields >> total;
        if (key == "MemAvailable:") fields >> available;
    }
    if (input.bad() || total <= 0 || available < 0 || available > total)
        throw std::runtime_error("Invalid /proc/meminfo");
    return static_cast<int16_t>(std::round(100.0 * (total - available) / total));
}

inline float uptimeDays(long seconds) { return seconds / 86400.0f; }
inline float get_uptime() {
    struct sysinfo info{};
    if (sysinfo(&info) < 0 || info.uptime < 0) throw std::runtime_error("Invalid uptime");
    return uptimeDays(info.uptime);
}
inline std::string get_modem_iface(const ReusableSysInfo& info) {
    return std::filesystem::exists(info.modem_path) ? info.modem_path : "NOT CONNECTED";
}
inline std::string get_modem_status(const ReusableSysInfo& info) {
    if (get_modem_iface(info) == "NOT CONNECTED") return "NOT CONNECTED";
    std::ifstream input(std::string(info.modem_path) + "/operstate");
    std::string state;
    if (!(input >> state)) throw std::runtime_error("Cannot read modem operstate");
    return state;
}
}
