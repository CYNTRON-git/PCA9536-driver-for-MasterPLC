#pragma once

#include "bind_opcua_base.hpp"
#include "share/lua/from_opcua_extension_object.h"

namespace mplc { namespace lua {

    // -------- OpcUa_ExtensionObject

    template<>
    struct Bind<detail::type_wrap<OpcUa_ExtensionObject, OpcUaType_ExtensionObject>> {
        template<class Tag>
        static int push(lua_State* L, OpcUa_ExtensionObject const& v, bool* ok = nullptr) {
            int len = 0;
            OpcUa_Byte* pOpcUaExtObjectValue = v.Body.Binary.Data;
            // len = CopyOpcUaBinaryObjectToLua(pOpcUaExtObjectValue, L, type);
            if (ok)
                *ok = true;
            return 1;
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, OpcUa_ExtensionObject& v, int idx = -1) {
            return OpcUa_BadTypeDefinitionInvalid;
            //MPLC_NOT_IMPLEMENTED(false);
        }
    };
}}  // namespace mplc::lua
