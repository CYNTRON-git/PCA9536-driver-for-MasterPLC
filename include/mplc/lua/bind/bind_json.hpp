#pragma once

#include <share/addins_share.h>
#include "bind_base.hpp"
#include "bind_string.hpp"
#include "mplc/lua/lua_writer.h"
#include <share/mplcshare.h>

namespace mplc { namespace lua {

    template<class WT>
    inline void get(lua_State* L, JsonWriter<WT>& writer, int i) {
        switch (lua_type(L, i)) {
        case LUA_TNIL:
            writer.Null();
            break;
        case LUA_TBOOLEAN:
            writer.Bool(lua_toboolean(L, -1));
            break;
        case LUA_TSTRING: {
            std::string u8str;
            lua::get(L, u8str, -1);
            writer.String(u8str);
        } break;
        case LUA_TNUMBER: {
            if (lua_isinteger(L, -1)) {
                writer.Int64(lua_tointeger(L, -1));
            } else {
                writer.Double(lua_tonumber(L, -1));
            }
        } break;
        case LUA_TTABLE: {
            int len = lua_rawlen(L, -1);
            if (len != 0) {
                writer.StartArray();
                for (int i = 1; i <= len; ++i) {
                    lua_rawgeti(L, -1, i);
                    lua::get(L, writer, -1);
                    lua_pop(L, 1);
                }
                writer.EndArray(len);
            } else {
                writer.StartObject();
                lua_pushnil(L);
                while (lua_next(L, -2) != 0) {
                    std::string u8str;
                    lua::get(L, u8str, -2);
                    writer.Key(u8str);
                    lua::get(L, writer, -1);
                    lua_pop(L, 1);
                }
                writer.EndObject();
            }
        } break;
        case LUA_TUSERDATA: {
            FileTime ft;
            if (OpcUa_IsGood(ft.from_lua(L, -1))) {
                writer.Int64(ft.dt());
            } else {
                writer.StartObject();
                writer.EndObject();
            }
        } break;
        case LUA_TLIGHTUSERDATA:
            writer.String("LUA_TLIGHTUSERDATA");
            break;
        case LUA_TFUNCTION:
            writer.String("LUA_TFUNCTION");
            break;
        case LUA_TTHREAD:
            writer.String("LUA_TTHREAD");
            break;
        default:
            break;
        }
    }

    namespace json {
        struct Wrapper {
            Value& data;
            Document::AllocatorType* allocator{};
            const vm::VmType* type{};
            GetValueFlags::GetValueFlagsEnum flags;
            Wrapper(OpcUa_Json& data,
                    const vm::VmType* type = nullptr,
                    GetValueFlags::GetValueFlagsEnum flags = GetValueFlags::None)
                : data(data.get()), allocator(&data.GetAllocator()), type(type), flags(flags) {}
            Wrapper(Value& data,
                    const vm::VmType* type = nullptr,
                    Document::AllocatorType* allocator = nullptr,
                    GetValueFlags::GetValueFlagsEnum flags = GetValueFlags::None)
                : data(data), allocator(allocator), type(type), flags(flags) {}
            Wrapper operator[](size_t n) const {
                auto el_type = type;
                if (el_type && type->as<vm::VmTypeArray>()) {
                    el_type = type->as<vm::VmTypeArray>()->elementType;
                }
                return Wrapper(data[n], el_type, allocator);
            }
            Wrapper wrap(Value& data, const vm::VmType* type = nullptr) const {
                return Wrapper(data, type ? type : this->type, allocator, flags);
            }
        };
        static Wrapper wrap(Value& data,
                            const vm::VmType* type = nullptr,
                            Document::AllocatorType* allocator = nullptr,
                            GetValueFlags::GetValueFlagsEnum flags = GetValueFlags::None) {
            return Wrapper(data, type, allocator, flags);
        }
        static Wrapper wrap(OpcUa_Json& data,
                            const vm::VmType* type = nullptr,
                            GetValueFlags::GetValueFlagsEnum flags = GetValueFlags::None) {
            return Wrapper(data, type, flags);
        }
    }  // namespace json

    template<>
    struct Bind<json::Wrapper> {
        template<class Tag>
        static int push(lua_State* L, const json::Wrapper& val, bool* ok = nullptr) {
            using namespace rapidjson;
            bool is_ok = true;
            switch (val.data.GetType()) {
            case kArrayType: {
                LuaWriter writer(L);
                writer.StartArray(val.data.Size());
                for (size_t i = 0; i < val.data.Size(); ++i) {
                    writer.Value<Tag>(val[i], &is_ok);
                }
                writer.EndArray();
                break;
            }

            case kNullType:
                if (val.type && !val.type->as<vm::VmTypeStruct>() && !val.type->as<vm::VmTypeArray>()) {
                    OpcUa_VariantHlp tmp;
                    if (OpcUa_IsBad(tmp.ChangeType(val.type->OpcType()))) {
                        lua_pushnil(L);
                        is_ok = false;
                        break;
                    }
                    return lua::set<Tag, OpcUa_Variant>(L, tmp, ok);
                }
                lua_pushnil(L);

                break;
            case kFalseType:
            case kTrueType: {
                if (val.type && val.type->OpcType() != OpcUaType_Boolean) {
                    OpcUa_VariantHlp tmp;
                    tmp.SetBool(kTrueType == val.data.GetType());
                    OpcUa_StatusCode sc = tmp.ChangeType(val.type->OpcType());
                    if (OpcUa_IsBad(sc)) {
                        lua_pushnil(L);
                        is_ok = false;
                        break;
                    }
                    return lua::set<Tag, OpcUa_Variant>(L, tmp, ok);
                }
                return lua::set<Tag>(L, kTrueType == val.data.GetType(), ok);
            }
            case kNumberType:
                if (!val.type || val.type->OpcTypeHlp().isNumber()) {
                    if (val.data.IsInt() || val.data.IsInt64())
                        return lua::set<Tag>(L, val.data.GetInt64(), ok);
                    else if (val.data.IsUint() || val.data.IsUint64())
                        return lua::set<Tag>(L, val.data.GetUint64(), ok);
                    else  //(value.IsDouble())
                        return lua::set<Tag>(L, val.data.GetDouble(), ok);
                } else if (val.type->OpcType() == OpcUaType_DateTime) {
                    int64_t rawtime = val.data.IsInt64()  ? val.data.GetInt64() :
                                      val.data.IsNumber() ? static_cast<int64_t>(val.data.GetDouble()) :
                                                            0;
                    FileTime ft = timetms_to_filetime(rawtime);
                    return lua::set<Tag>(L, ft, ok);
                } else {
                    OpcUa_VariantHlp tmp;
                    if (val.data.IsInt()) {
                        tmp.SetInt64(val.data.GetInt());
                    } else if (val.data.IsInt64()) {
                        tmp.SetInt64(val.data.GetInt64());
                    } else if (val.data.IsUint()) {
                        tmp.SetUInt64(val.data.GetUint());
                    } else if (val.data.IsUint64()) {
                        tmp.SetUInt64(val.data.GetUint64());
                    } else {
                        tmp.SetDouble(val.data.GetDouble());
                    }
                    OpcUa_StatusCode sc = tmp.ChangeType(val.type->OpcTypeHlp());
                    if (OpcUa_IsBad(sc)) {
                        lua_pushnil(L);
                        is_ok = false;
                        break;
                    }
                    return lua::set<Tag, OpcUa_Variant>(L, tmp, ok);
                }
                break;
            case kObjectType: {
                LuaWriter writer(L);
                writer.StartObject(val.data.MemberCount());
                for (auto it = val.data.MemberBegin(); it != val.data.MemberEnd(); ++it) {
                    const vm::VmType* type{};
                    lib::string_view field(it->name.GetString(), it->name.GetStringLength());
                    if (val.type) {
                        type = val.type->find(field);
                    }
                    writer.Field(field, val.wrap(it->value, type));
                }
                writer.EndObject();
                break;
            }
            case kStringType:
                if (!val.type || val.type->as<vm::VmTypeString>()) {
                    return lua::set<Tag>(L, lib::string_view(val.data.GetString(), val.data.GetStringLength()), ok);
                } else if (val.type->OpcType() == OpcUaType_DateTime) {
                    FileTime ft = _atoi64(val.data.GetString());
                    return lua::set<Tag>(L, ft, ok);
                } else {
                    OpcUa_VariantHlp tmp;
                    tmp.SetString(val.data.GetString(), val.data.GetStringLength());
                    OpcUa_StatusCode sc = tmp.ChangeType(val.type->OpcType());
                    if (OpcUa_IsBad(sc)) {
                        lua_pushnil(L);
                        is_ok = false;
                        break;
                    }
                    return lua::set<Tag, OpcUa_Variant>(L, tmp, ok);
                }
                break;
            default:
                break;
            }
            if (ok)
                *ok = is_ok;
            return 1;
        }
        static void get_string_key(lua_State* L,
                                   rapidjson::Value& val,
                                   rapidjson::Value::AllocatorType* allocator,
                                   int idx) {
            if (!allocator) {
                val.SetString("");
                return;
            }
            size_t str_len;
            const char* str = lua_tolstring(L, idx, &str_len);
            /*const int diff = ::cp1251_to_utf8_diff(str, str_len);
            if (diff > 0) {
                char* buf = val.ReservString(str_len + diff, *allocator);
                ::cp1251_to_utf8(buf, str, str_len);
            } else {
                val.SetString(str, str_len, *allocator);
            }*/
            val.SetString(str, str_len, *allocator);
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, json::Wrapper& val, int idx = -1) {
            switch (lua_type(L, idx)) {
            case LUA_TNIL:
                val.data.SetNull();
                break;
            case LUA_TBOOLEAN:
                val.data.SetBool(lua_toboolean(L, -1));
                break;
            case LUA_TSTRING: {
                get_string_key(L, val.data, val.allocator, idx);
                break;
            }
            case LUA_TNUMBER: {
                if (lua_isinteger(L, -1)) {
                    val.data.SetInt64(lua_tointeger(L, -1));
                } else {
                    val.data.SetDouble(lua_tonumber(L, -1));
                }
            } break;
            case LUA_TTABLE: {
                size_t len = lua_rawlen(L, -1);
                if (!val.allocator) {
                    bool is_arr = len > 0;
                    if (!is_arr) {
                        lua_pushnil(L);
                        is_arr = lua_next(L, -2) == 0;
                        if (is_arr) {
                            lua_pop(L, 1);
                        }
                    }
                    if (is_arr) {
                        val.data.SetArray();
                    } else {
                        val.data.SetObject();
                    }
                    break;
                }
                if (len > 0) {
                    val.data.SetArray();
                    val.data.Reserve(len, *val.allocator);
                    for (int i = 1; i <= len; ++i) {
                        lua_rawgeti(L, idx, i);
                        rapidjson::Value tmp;
                        auto wrap = val.wrap(tmp);
                        lua::get<Tag>(L, wrap, idx);
                        val.data.PushBack(tmp, *val.allocator);
                        lua_pop(L, 1);
                    }
                } else {
                    val.data.SetObject();
                    size_t count = 0;
                    lua_pushnil(L);
                    while (lua_next(L, -2) != 0) {
                        if (lua_type(L, -2) == LUA_TSTRING) {
                            Value key, tmp_val;
                            get_string_key(L, key, val.allocator, -2);
                            const vm::VmType* type{};
                            if (val.type) {
                                type = val.type->find(lib::string_view(key.GetString(), key.GetStringLength()));
                            }
                            auto wrap = val.wrap(tmp_val, type);
                            lua::get<Tag>(L, wrap, -1);
                            val.data.AddMember(key, tmp_val, *val.allocator);
                        }
                        ++count;

                        lua_pop(L, 1);
                    }
                    if (count == 0) {
                        val.data.SetArray();
                    }
                }
            } break;
            case LUA_TUSERDATA: {
                FILETIME* ptr = (FILETIME*)luaL_testudata(L, -1, "FileTime");
                if (ptr != nullptr) {
                    if ((val.flags & GetValueFlags::ReadTimeAsLinuxTime) != 0) {
                        int64_t t = filetime_to_timetms(int64_t(ptr->dwHighDateTime) << 32 | ptr->dwLowDateTime);
                        val.data.SetInt64(t);
                    } else
                        val.data.SetInt64(int64_t(ptr->dwHighDateTime) << 32 | ptr->dwLowDateTime);
                } else {
                    val.data.SetObject();
                }
            }
            case LUA_TLIGHTUSERDATA:
            case LUA_TFUNCTION:
            case LUA_TTHREAD:
            case LUA_NUMTAGS:
            default:
                break;
            }
            return OpcUa_Good;
        }
    };
    template<>
    struct Bind<rapidjson::Value> {
        template<class Tag>
        static int push(lua_State* L, const rapidjson::Value& val, bool* ok = nullptr) {
            return Bind<json::Wrapper>::push<Tag>(L, json::wrap(const_cast<rapidjson::Value&>(val)), ok);
        }
    };
    template<>
    struct Bind<OpcUa_Json> {
        template<class Tag>
        static int push(lua_State* L, const OpcUa_Json& val, bool* ok = nullptr) {
            return Bind<json::Wrapper>::push<Tag>(L, json::wrap(const_cast<OpcUa_Json&>(val)), ok);
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, OpcUa_Json& val, int idx = -1) {
            auto wrap = json::wrap(val);
            return Bind<json::Wrapper>::get<Tag>(L, wrap, idx);
        }
    };
    // template<class Tag, class T>
    // boost::enable_if_t<boost::is_same<rapidjson::Value, T>::type, OpcUa_StatusCode>  //
    //     get(lua_State* L, T& val, int idx = -1) {
    //     return Bind<json::Wrapper>::get<Tag>(L, json::wrap(val), idx);
    // }
    // template<class Tag, class T>
    // boost::enable_if_t<boost::is_same<rapidjson::Value, T>::type, int>  //
    //     set(lua_State* L, T& val, bool* ok = nullptr) {
    //     return Bind<json::Wrapper>::push<Tag>(L, json::wrap(val), ok);
    // }
}}  // namespace mplc::lua
