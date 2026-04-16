#pragma once
#include "bind_opcua_base.hpp"

namespace mplc { namespace lua {
    template<>
    struct Bind<detail::type_wrap<OpcUa_ExpandedNodeId, OpcUaType_ExpandedNodeId>> {
        template<class Tag>
        static int push(lua_State* L, OpcUa_ExpandedNodeId const& v, bool* ok = nullptr) {
            // TODO: Здесь не полный разбор. Пока выполнен только аналог разбор по аналогии с UAExpert
            switch (v.NodeId.IdentifierType) {
            case 1: {
                lua_pushfstring(L,
                                "%d|String|%s",
                                v.NodeId.NamespaceIndex,
                                OpcUa_String_GetRawString(&v.NodeId.Identifier.String));
                break;
            }
            default:
                lua_pushnil(L);
                break;
            }
            if (ok)
                *ok = true;
            return 1;
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, OpcUa_ExpandedNodeId& v, int idx = -1) {
            return OpcUa_BadTypeDefinitionInvalid;
            // MPLC_NOT_IMPLEMENTED(false);
        }
    };
}}  // namespace mplc::lua
