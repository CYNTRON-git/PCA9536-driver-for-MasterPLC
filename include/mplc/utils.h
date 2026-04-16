#pragma once
#include <cstddef>
#include <ostream>
#include "share/config.h"
#include "utils/fast_pimpl.h"
namespace mplc {
    template<class T, std::size_t N>
    constexpr std::size_t size(const T (&array)[N]) noexcept {
        return N;
    }
    namespace utils {
        enum class Unit {
            U_Byte,
            U_KiB,
            U_MiB,
            U_GiB,
            U_TiB,
        };
        inline const char* unit_name(Unit unit) {
            int uint_ = static_cast<int>(unit);
            static const char* units[] = {"B", "KiB", "MiB", "GiB", "TiB"};
            if (uint_ >= 0 && uint_ < size(units)) {
                return units[uint_];
            }
            return "UNK";
        }

        inline double format_bytes(double bytes, Unit& unit) {
            if (bytes < 1ll << 10) {
                unit = Unit::U_Byte;
                return bytes;
            }
            if (bytes < 1ll << 20) {
                unit = Unit::U_KiB;
                return bytes / (1ll << 10);
            }
            if (bytes < 1ll << 30) {
                unit = Unit::U_MiB;
                return bytes / (1ll << 20);
            }
            if (bytes < 1ll << 40) {
                unit = Unit::U_GiB;
                return bytes / (1ll << 30);
            }
            unit = Unit::U_TiB;
            return bytes / (1ll << 40);
        }
        MPLCSHARE_API void hex_dump(std::ostream& ss, const void* ptr, size_t len);

    }  // namespace utils

}  // namespace mplc
