#pragma once
//#include <share/mplcshare.h>
#include <share/filetime.h>
#include "bind_base.hpp"

template<>
struct mplc::lua::Bind<FILETIME> {
    template<class Tag>
    static int push(lua_State* L, const FILETIME& value, bool* ok = nullptr) {
        FILETIME* ud = (FILETIME*)lua_newuserdata(L, sizeof(FILETIME));
        luaL_getmetatable(L, "FileTime");
        lua_setmetatable(L, -2);
        std::memcpy(ud, &value, sizeof(FILETIME));
        if (ok)
            *ok = true;
        return 1;
    }
    template<class Tag>
    static OpcUa_StatusCode get(lua_State* L, FILETIME& value, int idx = -1) {
        // static_cast<int64_t>(ft.dwHighDateTime) << 32 | static_cast<int64_t>(ft.dwLowDateTime);
        switch (lua_type(L, idx)) {
        case LUA_TNUMBER:
            if (lua_isinteger(L, idx)) {
                int64_t ft = 0;
                lua::get<Tag>(L, ft, idx);
                std::memcpy(&value, &ft, sizeof(FILETIME));
            } else {
                double time = 0;
                lua::get<Tag>(L, time, idx);
                int64_t tmp = time * FT_MILLISECOND;
                std::memcpy(&value, &tmp, sizeof(FILETIME));
            }
            break;
        case LUA_TUSERDATA: {
            void* ud = luaL_checkudata(L, idx, "FileTime");  // luaL_checkudata(L, n, FileTimeName);
            if (ud == nullptr) {
                PRINTLN("'FILETIME' expected");
                return OpcUa_BadTypeMismatch;
            } else {
                value = *static_cast<FILETIME*>(ud);
            }
            return OpcUa_Good;
        }
        default:
            return OpcUa_BadTypeMismatch;
        }
        return OpcUa_Good;
    }
};

template<>
struct mplc::lua::Bind<FileTime> {
    template<class Tag>
    static int push(lua_State* L, const FileTime& value, bool* ok = nullptr) {
        value.to_lua(L);
        if (ok)
            *ok = true;
        return 1;
    }
    template<class Tag>
    static OpcUa_StatusCode get(lua_State* L, FileTime& value, int idx = -1) {
        switch (lua_type(L, idx)) {
        case LUA_TNUMBER:
            if (lua_isinteger(L, idx)) {
                int64_t ft = 0;
                lua::get<Tag>(L, ft, idx);
                value = ft;
            } else {
                double time = 0;
                lua::get<Tag>(L, time, idx);
                value = time;
            }
            break;
        case LUA_TUSERDATA:
            return value.from_lua(L, idx);
        default:
            return OpcUa_BadTypeMismatch;
        }
        return OpcUa_Good;
    }
};

template<>
inline int mplc::lua::Bind<FileTime>::template push<double>(lua_State* L, const FileTime& value, bool* ok) {
    return lua::set(L, value.into<double>(), ok);
}
template<>
inline int mplc::lua::Bind<FileTime>::template push<int64_t>(lua_State* L, const FileTime& value, bool* ok) {
    return lua::set(L, value.into<int64_t>(), ok);
}
