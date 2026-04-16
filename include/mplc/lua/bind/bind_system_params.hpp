#pragma once
#include "bind_base.hpp"
#include <mplc/api/system_params.h>
#include "share/lua/lua_addins.h"
#include <mplc/utils.h>
template<class T>
struct mplc::lua::Bind<mplc::api::system_param<T>> {
    template<class Tag>
    static int push(lua_State* L, const mplc::api::system_param<T>& value, bool* ok = nullptr) {
        lua_createtable(L, 0, 3);
        lua_pushlstring(L, SYSTEM_PARAM_TIME_FIELD, mplc::size(SYSTEM_PARAM_TIME_FIELD) - 1);
        lua::set<Tag>(L, value.SourceTime);
        lua_rawset(L, -3);

        bool is_ok = true;
        lua_pushlstring(L, SYSTEM_PARAM_VALUE_FIELD, mplc::size(SYSTEM_PARAM_VALUE_FIELD) - 1);
        lua::set<Tag>(L, value.Value, is_ok);
        if (!is_ok) {
            lua_pushnil(L);
        }
        lua_rawset(L, -3);

        lua_pushlstring(L, SYSTEM_PARAM_QUALITY_FIELD, mplc::size(SYSTEM_PARAM_QUALITY_FIELD) - 1);
        lua::set<Tag>(L, value.StatusCode);
        lua_rawset(L, -3);

        if (ok)
            *ok = true;
        return 1;
    }
    template<class Tag>
    static OpcUa_StatusCode get(lua_State* L, mplc::api::system_param<T>& value, int idx = -1) {
        if (::lua_type(L, idx) != LUA_TTABLE)
            return OpcUa_BadTypeMismatch;
        if (lua_getfield(L, idx, SYSTEM_PARAM_VALUE_FIELD) == LUA_TNIL) {
            lua_pop(L, 1);
            return OpcUa_BadTypeMismatch;
        }
        OpcUa_ReturnErrorIfBad(lua::get<Tag>(L, value.Value));
        lua_pop(L, 1);

        if (lua_getfield(L, idx, SYSTEM_PARAM_TIME_FIELD) != LUA_TUSERDATA) {
            lua_pop(L, 1);
            return OpcUa_BadTypeMismatch;
        }
        OpcUa_ReturnErrorIfBad(lua::get<Tag>(L, value.SourceTime));
        lua_pop(L, 1);

        if (lua_getfield(L, idx, SYSTEM_PARAM_QUALITY_FIELD) != LUA_TNUMBER) {
            lua_pop(L, 1);
            return OpcUa_BadTypeMismatch;
        }
        OpcUa_ReturnErrorIfBad(lua::get<Tag>(L, value.StatusCode));
        lua_pop(L, 1);
        return OpcUa_Good;
    }
};
