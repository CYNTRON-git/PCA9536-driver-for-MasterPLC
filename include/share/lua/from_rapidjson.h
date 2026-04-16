#pragma once
#include "type_traits.h"

MPLCSHARE_API OpcUa_StatusCode set_lua_value(const rapidjson::Value& value,
                                             lua_State* L,
                                             const OpcUa_BuiltInTypeHlp* type = nullptr);

MPLCSHARE_API OpcUa_StatusCode set_lua_value_any(const rapidjson::Value& value, lua_State* L);


MPLCSHARE_API void get_lua_value(Value& json,
                                 lua_State* L,
                                 Document::AllocatorType& allocator,
                                 GetValueFlags::GetValueFlagsEnum flags = GetValueFlags::None);


inline void set_lua_value(const rapidjson::Document& value, lua_State* L) {
    set_lua_value(static_cast<const Value&>(value), L);
}

inline void get_lua_value(rapidjson::Document& json, lua_State* L) {
    json.Clear();
    get_lua_value(json, L, json.GetAllocator(), GetValueFlags::None);
}
inline void get_lua_value(mplc::OpcUa_Json& json, lua_State* L) {
    json.Clear();
    get_lua_value(json.get(), L, json.GetAllocator(), GetValueFlags::None);
}

