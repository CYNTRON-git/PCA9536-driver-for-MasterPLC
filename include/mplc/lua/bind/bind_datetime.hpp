#pragma once
#include <mplc/date_time.h>
#include "bind_base.hpp"

template<>
struct mplc::lua::Bind<mplc::DateTime> {
    template<class Tag>
    static int push(lua_State* L, const DateTime& value, bool* ok = nullptr) {
        FILETIME* ud = (FILETIME*)lua_newuserdata(L, sizeof(FILETIME));
        luaL_getmetatable(L, "FileTime");
        lua_setmetatable(L, -2);
        *ud = value.into<FILETIME>();
        if (ok)
            *ok = true;
        return 1;
    }
    template<class Tag>
    static OpcUa_StatusCode get(lua_State* L, DateTime& value, int idx = -1) {
        switch (lua_type(L, idx)) {
        case LUA_TNUMBER: {
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
        }
        case LUA_TUSERDATA: {
            void* ud = luaL_checkudata(L, idx, "FileTime");
            if (ud == nullptr) {
                return OpcUa_BadTypeMismatch;
            }
            value = DateTime::from(*static_cast<FILETIME*>(ud));
            return OpcUa_Good;
        }
        default:
            return OpcUa_BadTypeMismatch;
        }
        return OpcUa_Good;
    }
};

template<>
inline int mplc::lua::Bind<mplc::DateTime>::template push<double>(lua_State* L, const mplc::DateTime& value, bool* ok) {
    return lua::set(L, value.into<double>(), ok);
}
template<>
inline int mplc::lua::Bind<mplc::DateTime>::template push<int64_t>(lua_State* L,
                                                                   const mplc::DateTime& value,
                                                                   bool* ok) {
    return lua::set(L, value.ticks(), ok);
}
