#pragma once
#include "bind_opcua_base.hpp"

template<>
struct mplc::lua::Bind<mplc::lua::detail::type_wrap<OpcUa_XmlElement, OpcUaType_XmlElement>> {
    template<class Tag>
    static int push(lua_State* L, const OpcUa_XmlElement& val, bool* ok = nullptr) {
        lua_pushlstring(L, (const char*)(val.Data), val.Length);
        if (ok)
            *ok = true;
        return 1;
    }
    template<class Tag>
    static OpcUa_StatusCode get(lua_State* L, OpcUa_XmlElement& value, int idx = -1) {
        return OpcUa_BadTypeDefinitionInvalid;
        // TODO: Add declaration
        //MPLC_NOT_IMPLEMENTED(false);
    }
};
