#pragma once
#include "bind_opcua_base.hpp"

namespace mplc { namespace lua {
    template<>
    struct Bind<detail::type_wrap<OpcUa_NodeId, OpcUaType_NodeId>> {
        template<class Tag>
        static int push(lua_State* L, OpcUa_NodeId const& v, bool* ok = nullptr) {
            switch (v.IdentifierType) {
            case 1:
                lua_pushfstring(L, "%d|String|%s", v.NamespaceIndex, OpcUa_String_GetRawString(&v.Identifier.String));
                break;
            default:
                lua_pushnil(L);
                break;
            }
            if (ok)
                *ok = true;
            return 1;
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, OpcUa_NodeId& v, int idx = -1) {
            return OpcUa_BadTypeDefinitionInvalid;
            //MPLC_NOT_IMPLEMENTED(false);
        }
    };
}}  // namespace mplc::lua
