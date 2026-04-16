#pragma once
#include <string>
#include <boost/utility/string_view.hpp>
#include <boost/container_hash/hash.hpp>
namespace mplc { namespace lib {
    using boost::string_view;
    inline string_view make_string_view(const void* data, size_t len) {
        return string_view(static_cast<const char*>(data), len);
    }
    struct boost_string_view_comparator {
        bool operator()(std::string const& lhs, std::string const& rhs) const {
            return lhs < rhs;
        }
        bool operator()(boost::string_view lhs, std::string const& rhs) const {
            return lhs < rhs;
        }
        bool operator()(std::string const& lhs, boost::string_view rhs) const {
            return lhs < rhs;
        }
    };
    struct string_hash {
        using is_transparent = int;
        std::size_t operator()(std::string const& v) const {
            return boost::hash_value(v);
        }
        std::size_t operator()(lib::string_view v) const {
            return boost::hash_value(v);
        }
    };

    struct string_equal_to {
        using is_transparent = int;
        constexpr bool operator()(lib::string_view _Left, const std::string& _Right) const {
            return _Left == _Right;
        }
        constexpr bool operator()(const std::string& _Left, lib::string_view _Right) const {
            return _Left == _Right;
        }
        constexpr bool operator()(const std::string& _Left, const std::string& _Right) const {
            return _Left == _Right;
        }
    };

}}  // namespace mplc::lib

inline std::string& operator+=(std::string& dst, mplc::lib::string_view src) {
    dst.append(src.data(), src.size());
    return dst;
}
