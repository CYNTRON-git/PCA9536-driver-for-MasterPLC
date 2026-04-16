#pragma once
#include <mplc/utils.h>
#include "bind_opcua_base.hpp"
#include "../bind_filetime.hpp"
namespace mplc { namespace lua {
    template<>
    struct Bind<detail::type_wrap<OpcUa_DataValue, OpcUaType_DataValue>> {
        template<class Tag>
        static int push(lua_State* L, OpcUa_DataValue const& v, bool* ok = nullptr) {
            bool is_ok = true;
            LuaWriter writer(L);
            writer.StartObject(3);
            writer.Field<Tag>(SYSTEM_PARAM_VALUE_FIELD, v.Value, &is_ok);
            if (!is_ok) {
                lua_pushstring(L, SYSTEM_PARAM_VALUE_FIELD);
                lua_pushnil(L);
                lua_rawset(L, -3);
            }
            writer.Field<Tag>(SYSTEM_PARAM_TIME_FIELD, v.SourceTimestamp, &is_ok);
            writer.Field<Tag>(SYSTEM_PARAM_QUALITY_FIELD, v.StatusCode, &is_ok);
            writer.EndObject();
            if (ok)
                *ok = is_ok;
            return 1;
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, OpcUa_DataValue& v, int idx = -1) {
            if (::lua_type(L, idx) != LUA_TTABLE)
                return OpcUa_BadTypeMismatch;
            if (lua_getfield(L, idx, SYSTEM_PARAM_VALUE_FIELD) == LUA_TNIL) {
                lua_pop(L, 1);
                return OpcUa_BadTypeMismatch;
            }
            OpcUa_ReturnErrorIfBad(lua::get<Tag>(L, v.Value));
            lua_pop(L, 1);

            if (lua_getfield(L, idx, SYSTEM_PARAM_TIME_FIELD) != LUA_TUSERDATA) {
                lua_pop(L, 1);
                return OpcUa_BadTypeMismatch;
            }
            OpcUa_ReturnErrorIfBad(lua::get<Tag>(L, v.ServerTimestamp));
            lua_pop(L, 1);

            if (lua_getfield(L, idx, SYSTEM_PARAM_QUALITY_FIELD) != LUA_TNUMBER) {
                lua_pop(L, 1);
                return OpcUa_BadTypeMismatch;
            }
            OpcUa_ReturnErrorIfBad(lua::get<Tag>(L, v.StatusCode));
            lua_pop(L, 1);
            return OpcUa_Good;
        }
    };
}}  // namespace mplc::lua
