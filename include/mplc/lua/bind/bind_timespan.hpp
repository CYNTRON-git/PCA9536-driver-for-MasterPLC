#pragma once
#include <mplc/time_span.h>
#include "bind_base.hpp"

template<>
struct mplc::lua::Bind<mplc::TimeSpan> {
    template<class Tag>
    static int push(lua_State* L, const TimeSpan& value, bool* ok = nullptr) {
        lua_pushnumber(L, value.msec_f());
        if (ok)
            *ok = true;
        return 1;
    }
    template<class Tag>
    static OpcUa_StatusCode get(lua_State* L, TimeSpan& value, int idx = -1) {
        switch (lua_type(L, idx)) {
        case LUA_TNUMBER:
            if (lua_isinteger(L, idx)) {
                value = lua_tointegerx(L, idx, nullptr);
            } else {
                value = lua_tonumberx(L, idx, nullptr);
            }
            break;
        default:
            return OpcUa_BadTypeMismatch;
        }
        return OpcUa_Good;
    }
};

template<>
inline int mplc::lua::Bind<mplc::TimeSpan>::template push<double>(lua_State* L, const TimeSpan& value, bool* ok) {
    return lua::set(L, value.msec_f(), ok);
}
template<>
inline int mplc::lua::Bind<mplc::TimeSpan>::template push<int64_t>(lua_State* L, const TimeSpan& value, bool* ok) {
    return lua::set(L, value.ticks(), ok);
}
