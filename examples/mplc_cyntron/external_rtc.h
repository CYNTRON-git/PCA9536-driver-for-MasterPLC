#pragma once
#include <chrono>
#include <filesystem>
#include <linux/rtc.h>
#include <optional>

namespace mplc::cyntron {
class ExternalRtc {
public:
    // Read the battery-backed clock in UTC. Poll at the hardware's one-second resolution.
    rtc_time read();
private:
    std::chrono::steady_clock::time_point next_{};
    std::optional<rtc_time> sample_;
};

// The sysfs root parameter permits testing the same selection code on different board layouts.
rtc_time readExternalRtc(const std::filesystem::path& sysfs);
}
