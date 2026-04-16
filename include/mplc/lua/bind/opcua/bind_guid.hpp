#pragma once
#include "bind_opcua_base.hpp"

namespace mplc { namespace lua {

    template<>
    struct Bind<detail::type_wrap<OpcUa_Guid, OpcUaType_Guid>> {
        template<class Tag>
        static int push(lua_State* L, OpcUa_Guid const& v, bool* ok = nullptr) {
            // lua_pushfstring(L, "{%x-%x-%x-%x}", v->Data1, v->Data2, v->Data3, *v->Data4);
            if (ok)
                *ok = true;
            return 1;
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, OpcUa_Guid& v, int idx = -1) {
            return OpcUa_BadTypeDefinitionInvalid;
            //MPLC_NOT_IMPLEMENTED(false);
        }
    };

}}  // namespace mplc::lua
