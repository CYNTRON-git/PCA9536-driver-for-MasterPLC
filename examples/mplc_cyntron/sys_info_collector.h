#pragma once
#include <stdexcept>
#include <cstring>
#include <string>
#include <cmath>
#include <cstdint>
#include "core/main.h"
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <sys/time.h>
#include <linux/rtc.h>
#include <stdlib.h>
#include <net/if.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <filesystem>

namespace mplc { namespace cyntron {

    struct MediaData {
        bool mounted;
        float size;
        float used;
    };
    template<typename Func>
    struct defer {
        Func f;
        explicit defer(Func f): f(std::move(f)) {}
        ~defer() {
            f();
        }
    };

    constexpr const char* sd_mountpoint = "/media/sdcard";
    constexpr const char* usb_mountpoint = "/media/usb";

    MediaData get_fs_media_data(const char* path) {
        MediaData res;
        struct statvfs stvfs;
        if (statvfs(path, &stvfs) < 0) {
            char buf[256] = "";
            throw std::runtime_error((sprintf(buf, "Не удалось открыть %s", path), buf));
        } else {
            return {true,
                    (int64_t)stvfs.f_blocks * (int64_t)stvfs.f_frsize / 1024.0f / 1024.0f / 1024.0f,
                    ((int64_t)stvfs.f_blocks - (int64_t)stvfs.f_bavail) * (int64_t)stvfs.f_frsize / 1024.0f / 1024.0f /
                        1024.0f};
        }
    }

    struct ReusableSysInfo {
        bool fetched = false;
        bool modem_state = false;
        float ram_percent;
        float uptime;
        MediaData usb;
        MediaData sd_card;

        void check_mount() {
            FILE* fp = fopen("/proc/mounts", "r");
            if (!fp) {
                throw std::runtime_error("не удалось открыть /proc/mounts");
            }

            char device[256], path[256], fstype[64], opts[256];
            int freq, passno;
            bool mounted = false;
            usb = {false};
            sd_card = {false};
            while (fscanf(fp, "%255s %255s %63s %255s %d %d", device, path, fstype, opts, &freq, &passno) == 6) {
                if (strcmp(path, sd_mountpoint) == 0) {
                    sd_card = {true};
                }
                if (strcmp(path, usb_mountpoint) == 0) {
                    usb = {true};
                }
            }
            fclose(fp);
        }

        void fetch() {
            struct sysinfo si;
            if (sysinfo(&si) < 0) {
                throw std::runtime_error(std::string("Не удалось получить информацию о системе ") +
                                         std::strerror(errno));
            }
            uptime = si.uptime / 60.0f / 24.0f;
            check_mount();
            if (usb.mounted) {
                usb = get_fs_media_data(usb_mountpoint);
            }

            if (sd_card.mounted) {
                sd_card = get_fs_media_data(sd_mountpoint);
            }

            fetched = true;
        }
    };

    inline int16_t get_cpu_temp() {
        int fd = open("/sys/class/thermal/thermal_zone0/temp", O_RDONLY);
        defer fd_close{[&fd]() { close(fd); }};
        if (fd < 0) {
            throw std::runtime_error(std::string("Не удалось открыть /sys/class/thermal/thermal_zone0/temp, error = ") +
                                     std::strerror(errno));
        }
        char buf[20];
        if (read(fd, buf, sizeof(buf)) < 0) {
            throw std::runtime_error(
                std::string("Не удалось чтение из /sys/class/thermal/thermal_zone0/temp, error = ") +
                std::strerror(errno));
        };
        float res;
        sscanf(buf, "%f", &res);
        return std::round(res * 0.001f);
    }

    inline int16_t get_ram_percent(ReusableSysInfo& sys_info) {
        std::ifstream meminfo_f("/proc/meminfo");
        if (!meminfo_f.is_open()) {
            throw std::runtime_error("Не удалось открыть /proc/meminfo");
        }
        std::string line;
        std::string key, value;
        long long mem_avail = -1;
        long long mem_total = -1;
        while (!meminfo_f.eof()) {
            std::getline(meminfo_f, line);
            std::stringstream line_stream(line);
            line_stream >> key >> value;
            if (key == "MemAvailable:") {
                mem_avail = std::stoll(value);
            }
            if (key == "MemTotal:") {
                mem_total = std::stoll(value);
            }
        }
        if (mem_avail == -1 || mem_total == -1) {
            throw std::runtime_error("В /proc/meminfo отсутсвует MemAvailable либо MemTotal");
        }

        return std::round((1.0f - (float)mem_avail / mem_total) * 100.0f);
    }

    inline int16_t get_disk_percent() {
        // По неизвестным причинам код ниже не работает на цинтроне, заменю на вызов df
        struct statvfs stvfs;
        statvfs("/", &stvfs);
        return std::round((1.0f - ((int64_t)stvfs.f_bavail * (int64_t)stvfs.f_frsize) /
                                      (float)((int64_t)stvfs.f_blocks * (int64_t)stvfs.f_frsize)) *
                          100.0f);
    }

    inline float get_uptime(ReusableSysInfo& sys_info) {
        sys_info.fetch();
        return sys_info.uptime;
    }

    inline mplc::DateTime get_sys_time() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return mplc::DateTime::from_unix(tv.tv_sec);
    }

    inline mplc::DateTime get_rtc_time() {
        struct rtc_time res;
        int fd = open("/dev/rtc0", O_RDONLY);
        defer fd_close{[&fd]() { close(fd); }};
        if (fd < 0) {
            throw std::runtime_error(std::string("Не удалось открыть часы реального времени /dev/rtc0 ") +
                                     std::strerror(errno));
        }
        if (ioctl(fd, RTC_RD_TIME, &res) < 0) {
            throw std::runtime_error(std::string("Запрос к часам реального времени завершен с ошибкой: ") +
                                     std::strerror(errno));
        }
        return mplc::DateTime(1900 + res.tm_year, res.tm_mon + 1, res.tm_mday, res.tm_hour, res.tm_min, res.tm_sec);
    }

    inline std::string get_modem_status(ReusableSysInfo& sys_info) {
        sys_info.modem_state = false;
        std::ifstream modem_state("/sys/class/net/usb0/operstate", std::ios::in);
        if (!modem_state.is_open()) {
            return "NOT CONNECTED";
        }
        std::string status;
        sys_info.modem_state = true;
        modem_state >> status;
        return status;
    }

    inline std::string get_modem_iface(ReusableSysInfo& sys_info) {
        return sys_info.modem_state ? "/sys/class/net/usb0" : "NOT CONNECTED";
    }

    inline bool get_usb_flash_dev(ReusableSysInfo& sys_info) {
        sys_info.fetch();
        return sys_info.usb.mounted;
    }

    inline float get_usb_flash_size(ReusableSysInfo& sys_info) {
        sys_info.fetch();
        return sys_info.usb.mounted ? sys_info.usb.size : 0;
    }

    inline float get_usb_flash_used(ReusableSysInfo& sys_info) {
        sys_info.fetch();
        return sys_info.usb.mounted ? sys_info.usb.used : 0;
    }

    inline int16_t get_usb_flash_perc(ReusableSysInfo& sys_info) {
        sys_info.fetch();
        return sys_info.usb.mounted ? std::round(sys_info.usb.used / sys_info.usb.size * 100.0f) : 0;
    }

    inline bool get_sd_card_dev(ReusableSysInfo& sys_info) {
        sys_info.fetch();
        return sys_info.sd_card.mounted;
    }

    inline float get_sd_card_size(ReusableSysInfo& sys_info) {
        sys_info.fetch();
        return sys_info.sd_card.mounted ? sys_info.sd_card.size : 0;
    }

    inline float get_sd_card_used(ReusableSysInfo& sys_info) {
        sys_info.fetch();
        return sys_info.sd_card.mounted ? sys_info.sd_card.used : 0;
    }

    inline int16_t get_sd_card_perc(ReusableSysInfo& sys_info) {
        sys_info.fetch();
        return sys_info.sd_card.mounted ? std::round(sys_info.sd_card.used / sys_info.sd_card.size * 100.0f) : 0;
    }

}}  // namespace mplc::cyntron
