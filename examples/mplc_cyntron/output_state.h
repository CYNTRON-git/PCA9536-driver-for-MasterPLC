#pragma once

#include <charconv>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string_view>

namespace mplc::cyntron {
inline uint8_t applyOverride(uint8_t mask, std::string_view text, int64_t now) {
    std::optional<int> value;
    std::optional<int64_t> expires;
    while (!text.empty()) {
        const auto end = text.find('\n');
        const auto line = text.substr(0, end);
        text = end == text.npos ? std::string_view{} : text.substr(end + 1);
        if (line == "value=0" || line == "value=1") {
            if (value) return mask;
            value = line.back() - '0';
        } else if (line.substr(0, 11) == "expires_at=") {
            int64_t epoch;
            auto last = line.data() + line.size();
            auto parsed = std::from_chars(line.data() + 11, last, epoch);
            if (expires || parsed.ec != std::errc{} || parsed.ptr != last) return mask;
            expires = epoch;
        } else {
            return mask;
        }
    }
    if (!value || !expires || now < 0 || *expires <= now) return mask;
    return *value ? mask & ~4u : mask | 4u;
}

class OutputState {
public:
    using Clock = std::chrono::steady_clock;
    template<class Sync>
    bool update(uint8_t mask, Clock::time_point now, Sync sync) {
        if (now < next_check && (failed || confirmed == mask)) return !failed;
        try {
            sync(mask);
        } catch (...) {
            failed = true;
            next_check = now + std::chrono::seconds(1);
            throw;
        }
        confirmed = mask;
        failed = false;
        next_check = now + std::chrono::seconds(1);
        return true;
    }
private:
    std::optional<uint8_t> confirmed;
    bool failed = false;
    Clock::time_point next_check{};
};
}
