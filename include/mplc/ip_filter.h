#pragma once
#include <vector>
#include <array>
#include <mplc/libs/string_view.hpp>
#include <share/config.h>
namespace mplc {
    struct IpFilter {
        struct ParseError {
            size_t pos{};
            lib::string_view error;
            operator bool() const noexcept {
                return !error.empty();
            }
        };
        struct Rang {
            uint8_t from{}, to{};
            Rang(uint8_t from_, uint8_t to_): from(from_), to(to_) {}
            bool contains(uint8_t v) const noexcept {
                return v >= from && v <= to;
            }
            bool operator==(const Rang& other) const noexcept {
                return from == other.from && to == other.to;
            }
            bool operator!=(const Rang& other) const noexcept {
                return !(*this == other);
            }
        };
        using Part = std::vector<Rang>;
        std::array<Part, 4> parts;
        MPLCSHARE_API void init(lib::string_view ip, ParseError& ec);
        MPLCSHARE_API bool contains(lib::string_view ip, ParseError& ec) const noexcept;
        MPLCSHARE_API std::string to_string() const;
        bool operator==(const IpFilter& other) const noexcept {
            return parts == other.parts;
        }
        bool operator!=(const IpFilter& other) const noexcept {
            return !(*this == other);
        }
    };

    MPLCSHARE_API std::vector<IpFilter> parse_addresses(lib::string_view ip_addresses,
                                                        IpFilter::ParseError* ec = nullptr);

    class IpWhiteList {
        std::vector<IpFilter> filters_;

    public:
        MPLCSHARE_API IpWhiteList() = default;
        MPLCSHARE_API IpWhiteList(std::vector<IpFilter> filters): filters_(std::move(filters)) {}
        MPLCSHARE_API bool filter(lib::string_view ip) const;
        MPLCSHARE_API std::string to_string() const;
        MPLCSHARE_API bool operator==(const IpWhiteList& other) const noexcept {
            return filters_ == other.filters_;
        }
        MPLCSHARE_API bool operator!=(const IpWhiteList& other) const noexcept {
            return !(*this == other);
        }
        MPLCSHARE_API bool empty() const noexcept {
            return filters_.empty();
        }
    };

}  // namespace mplc
