#pragma once
#include "bind_opcua_base.hpp"

namespace mplc { namespace lua {
    template<>
    struct Bind<detail::type_wrap<OpcUa_ByteString, OpcUaType_ByteString>> {
        template<class Tag>
        static int push(lua_State* L, OpcUa_ByteString const& v, bool* ok = nullptr) {
            lua_createtable(L, v.Length, 0);
            for (decltype(v.Length) i = 0; i < v.Length; i++) {
                lua_pushnumber(L, i + 1);
                lua_pushinteger(L, v.Data[i]);
                lua_rawset(L, -3);
            }
            if (ok)
                *ok = true;
            return 1;
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, OpcUa_ByteString& v, int idx = -1) {
            size_t len;
            const char* s = lua_tolstring(L, idx, &len);
            OpcUa_ByteString_Initialize(&v);
            v.Length = len;
            v.Data = (OpcUa_Byte*)OpcUa_Alloc(len);
            OpcUa_Memory_MemCpy(v.Data, len, (void*)s, len);
            return OpcUa_Good;
        }
    };
}}  // namespace mplc::lua
