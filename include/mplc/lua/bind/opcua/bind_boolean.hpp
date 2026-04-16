#pragma once
#include "bind_opcua_base.hpp"

template<>
struct mplc::lua::Bind<mplc::lua::detail::type_wrap<OpcUa_Boolean, OpcUaType_Boolean>> {
    template<class Tag>
    static int push(lua_State* L, const OpcUa_Boolean& val, bool* ok = nullptr) {
        lua_pushboolean(L, (bool)val);
        if (ok)
            *ok = true;
        return 1;
    }
    template<class Tag>
    static OpcUa_StatusCode get(lua_State* L, OpcUa_Boolean& value, int idx = -1) {
        value = lua_toboolean(L, idx);
        return OpcUa_Good;
    }
};
