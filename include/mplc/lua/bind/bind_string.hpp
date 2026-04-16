#pragma once
#include <mplc/libs/string_view.hpp>
#include <mplc/libs/containers.hpp>
#include <share/opcua_variant_hlp.h>
#include <share/string_conv.h>
#include "bind_base.hpp"
namespace mplc { namespace lua {
    namespace detail {
        inline bool need_decode_to_cp1251(lib::string_view str) {
            if (str.empty())
                return false;
            if (str.size() >= 4) {
                for (size_t i = 0; i < str.size() / 4; ++i) {
                    const uint32_t* ch = reinterpret_cast<const uint32_t*>(&str.data()[i * 4]);
                    if (*ch & 0x80808080) {
                        return true;
                    }
                }
            }
            for (size_t i = str.size() - str.size() % 4; i < str.size(); ++i) {
                if (str[i] & 0x80) {
                    return true;
                }
            }
            return false;
        }
    }  // namespace detail
    namespace encoding {
        class UTF8_t {};
        class CP1251_t {};
    }  // namespace encoding

    // ------ lib::string_view
    template<>
    struct Bind<lib::string_view> {
        template<class Tag>
        static int push(lua_State* L, lib::string_view val, bool* ok = nullptr) {
            lua_pushlstring(L, val.data(), val.size());
            if (ok)
                *ok = true;
            return 1;
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, lib::string_view& val, int i = -1) {
            size_t str_len;
            const char* str = lua_tolstring(L, i, &str_len);
            val = lib::string_view(str, str_len);
            return OpcUa_Good;
        }
    };

    template<>
    inline int Bind<lib::string_view, void>::push<encoding::CP1251_t>(lua_State* L, lib::string_view val, bool* ok) {
        const size_t diff = cp1251_to_utf8_diff(val.data(), val.size());
        if (diff > 0) {
            char* buf = new char[val.size() + diff + 1];
            const size_t new_size = ::cp1251_to_utf8(buf, val.data(), val.size());
            lua_pushlstring(L, buf, new_size);
            delete[] buf;
        } else {
            lua_pushlstring(L, val.data(), val.size());
        }
        if (ok)
            *ok = true;
        return 1;
    }
    template<>
    struct Bind<std::string> {
        template<class Tag>
        static int push(lua_State* L, const std::string& val, bool* ok = nullptr) {
            return Bind<lib::string_view>::push<Tag>(L, val, ok);
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, std::string& val, int i = -1) {
            size_t str_len;
            const char* str = lua_tolstring(L, i, &str_len);
            val.assign(str, str_len);
            return OpcUa_Good;
        }
    };

    template<>
    inline OpcUa_StatusCode Bind<std::string, void>::get<encoding::CP1251_t>(lua_State* L, std::string& val, int i) {
        size_t str_len;
        const char* str = lua_tolstring(L, i, &str_len);
        if(!str || !str_len) {
            val.clear();
            return OpcUa_Good;
        }
        if (need_decode_to_cp1251(str)) {
            val.resize(str_len);
            auto new_size = ::utf8_to_cp1251(str, &val[0], str_len);
            val.resize(new_size);
        } else {
            val.assign(str, str_len);
        }
        return OpcUa_Good;
    }

}}  // namespace mplc::lua
