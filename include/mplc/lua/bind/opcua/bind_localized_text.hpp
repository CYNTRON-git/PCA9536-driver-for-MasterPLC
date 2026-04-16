#pragma once

#include "bind_opcua_base.hpp"

namespace mplc { namespace lua {
    template<>
    struct Bind<detail::type_wrap<OpcUa_LocalizedText, OpcUaType_LocalizedText>> {
        template<class Tag>
        static int push(lua_State* L, OpcUa_LocalizedText const& v, bool* ok = nullptr) {
            return set<Tag>(L, lib::string_view(OpcUa_String_GetRawString(&v.Text), OpcUa_String_StrSize(&v.Text)), ok);
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, OpcUa_LocalizedText& v, int idx = -1) {
            return OpcUa_BadTypeDefinitionInvalid;
            //MPLC_NOT_IMPLEMENTED(false);
        }
    };
}}  // namespace mplc::lua
