#pragma once
#include "bind_opcua_base.hpp"
#include "../bind_string.hpp"

namespace mplc { namespace lua {

    template<>
    struct Bind<detail::type_wrap<OpcUa_String, OpcUaType_String>> {
        template<class Tag>
        static int push(lua_State* L, OpcUa_String const& v, bool* ok = nullptr) {
            return set(L, lib::string_view(OpcUa_String_GetRawString(&v), OpcUa_String_StrSize(&v)), ok);
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, OpcUa_String& v, int idx = -1) {
            OpcUa_String_Initialize(&v);
            size_t len;
            const char* str = lua_tolstring(L, idx, &len);
            if (str == nullptr || len == 0) {
                return OpcUa_Good;
            }
            return OpcUa_String_AttachToString(const_cast<char*>(str),
                                               static_cast<OpcUa_UInt32>(len),
                                               0,
                                               OpcUa_True,
                                               OpcUa_True,
                                               &(v));
        }
    };

}}  // namespace mplc::lua
