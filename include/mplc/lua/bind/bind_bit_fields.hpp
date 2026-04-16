#pragma once
#include "bind_base.hpp"
#include <mplc/api/bit_types.h>

template<class T>
struct mplc::lua::Bind<mplc::api::bit_type<T>, boost::enable_if_t<boost::is_unsigned<T>::type>> {
    template<class Tag>
    static int push(lua_State* L, const mplc::api::bit_type<T>& value, bool* ok = nullptr) {
        return lua::set<Tag>(L, static_cast<T>(value), ok);
    }
    template<class Tag>
    static void get(lua_State* L, mplc::api::bit_type<T>& value, int idx = -1) {
        if (::lua_type(L, idx) != LUA_TNUMBER)
            return;
        if (lua_isinteger(L, idx)) {
            T tmp;
            lua::get<Tag>(L, tmp, idx);
            value = tmp;
        }
    }
};
