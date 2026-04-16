#pragma once
#include "type_traits.h"

MPLCSHARE_API OpcUa_StatusCode set_lua_value(const OpcUa_Variant& v, lua_State* L);
inline OpcUa_StatusCode set_lua_value(const OpcUa_VariantHlp& v, lua_State* L) {
    return set_lua_value(static_cast<const OpcUa_Variant>(v), L);
}
MPLCSHARE_API OpcUa_StatusCode set_lua_value(const OpcUa_VariantHlp& value,
                                             lua_State* L,
                                             const OpcUa_BuiltInTypeHlp& type);
MPLCSHARE_API void get_lua_value(OpcUa_VariantHlp& value, lua_State* L, int idx = -1);

MPLCSHARE_API OpcUa_StatusCode set_lua_value(const OpcUa_DataValue& v, lua_State* L);
MPLCSHARE_API OpcUa_StatusCode set_lua_value(const OpcUa_DateTime& v, lua_State* L);

//template<class T>
//OpcUa_StatusCode set_lua_value(const T* from, OpcUa_Int32 len, lua_State* L) {
//    lua_createtable(L, 0, len);
//
//    for (OpcUa_Int32 i = 0; i < len; ++i) {
//        set_lua_value(i + 1, L);
//        set_lua_value(from[i], L);
//        lua_rawset(L, -3);
//    }
//    return OpcUa_Good;
//}

