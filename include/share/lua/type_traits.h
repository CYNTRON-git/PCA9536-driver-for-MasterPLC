#pragma once
#include <boost/type_traits/make_void.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <mplc/libs/string_view.hpp>
#include <mplc/libs/containers.hpp>
#include <share/mplcshare.h>
#include <share/string_conv.h>
#include <share/filetime.h>
#include "type_traits_macros.h"
#include "../opcua_json.h"
#include <stack>
#include "from_rapidjson.h"
#include "from_opcua_variant.h"

template<typename>
struct is_std_vector : boost::false_type {};
template<typename T>
struct is_std_vector<std::vector<T> > : boost::true_type {};
template<typename T>
struct is_pair : boost::false_type {};
template<typename T, typename U>
struct is_pair<std::pair<T, U> > : boost::true_type {};

template<typename, typename = void>
struct is_simple_wrapper : boost::false_type {};
template<typename T>
struct is_simple_wrapper<T, typename boost::make_void<typename T::lua_type>::type> : boost::true_type {};

template<typename, typename = void>
struct is_iterable : boost::false_type {};
template<typename T>
struct is_iterable<T, typename boost::make_void<typename T::const_iterator>::type> : boost::true_type {};

template<typename, typename = void>
struct is_self_get_set : boost::false_type {};
template<typename T>
struct is_self_get_set<T, typename boost::make_void<typename T::self_get_set>::type> : boost::true_type {};

template<typename T>
struct is_lua_proxy_t : boost::is_base_of<mplc::lua_proxy_t, T> {};

template<class T>
void set_lua_value(const T& value, lua_State* L);
template<class T>
void get_lua_value(T& value, lua_State* L);

struct RefTo : mplc::vm::ItemID {
    RefTo() {}
    RefTo(const std::string& id_path): ItemID(id_path), raw(id_path) {}
    RefTo(const ItemID& item): ItemID(item) {}
    explicit RefTo(int64_t id): ItemID(id, "") {}
    std::string raw;
};
MPLCSHARE_API bool test_lua_value(lua_State* L, int type, int idx = -1);

MPLCSHARE_API void set_lua_value(const RefTo& value, lua_State* L);
MPLCSHARE_API void get_lua_value(RefTo& value, lua_State* L);
MPLCSHARE_API void set_lua_value(const mplc::vm::ObjectID& value, lua_State* L);
MPLCSHARE_API void get_lua_value(mplc::vm::ObjectID& value, lua_State* L);
// MPLCSHARE_API void set_lua_u8string(const char* utf8, size_t size, lua_State* L);
// MPLCSHARE_API void get_lua_u8string(std::string& val, lua_State* L, int pos = -1);
// inline void set_lua_u8string(const std::string& val, lua_State* L) {
//     set_lua_u8string(val.c_str(), val.size(), L);  //-V107
// }
inline void set_lua_string(const char* s, int size, lua_State* L) {
    lua_pushlstring(L, s, size);
}
inline void set_lua_value(const mplc::lib::string_view& value, lua_State* L) {
    lua_pushlstring(L, value.data(), value.size());
}
inline void set_lua_value(const std::string& value, lua_State* L) {
    lua_pushlstring(L, value.c_str(), value.size());
}
inline void set_lua_value(const char* value, lua_State* L) {
    lua_pushstring(L, value);
}
inline void get_lua_value(std::string& value, lua_State* L, int pos = -1) {
    size_t len;
    const char* ch_str = lua_tolstring(L, pos, &len);
    value.assign(ch_str, len);
}
inline void get_lua_value(boost::string_view& value, lua_State* L) {
    size_t len;
    const char* ch_str = lua_tolstring(L, -1, &len);
    value = boost::string_view(ch_str, len);
}
inline void set_lua_value(const bool& value, lua_State* L) {
    lua_pushboolean(L, value);
}
inline void get_lua_value(bool& value, lua_State* L) {
    value = lua_toboolean(L, -1) != 0;
}

inline void set_lua_value(const FileTime& value, lua_State* L) {
    value.to_lua(L);
}
inline void get_lua_value(FileTime& value, lua_State* L) {
    value.from_lua(L, -1);
}

template<typename WT>
inline void get_lua_value(JsonWriter<WT>& writer, lua_State* L) {
    switch (lua_type(L, -1)) {
    case LUA_TNIL:
        writer.Null();
        break;
    case LUA_TBOOLEAN:
        writer.Bool(lua_toboolean(L, -1));
        break;
    case LUA_TSTRING: {
        std::string u8str;
        get_lua_value(u8str, L);
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
        int len = luaL_getn(L, -1);
        if (len != 0) {
            writer.StartArray();
            for (int i = 1; i <= len; ++i) {
                lua_rawgeti(L, -1, i);
                get_lua_value(writer, L);
                lua_pop(L, 1);
            }
            writer.EndArray(len);
        } else {
            writer.StartObject();
            lua_pushnil(L);
            while (lua_next(L, -2) != 0) {
                std::string u8str;
                get_lua_value(u8str, L, -2);
                writer.Key(u8str);
                get_lua_value(writer, L);
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

template<typename WT>
inline void get_lua_value_any(JsonWriter<WT>& writer, lua_State* L) {
    switch (lua_type(L, -1)) {
    case LUA_TNIL:
        writer.Null();
        break;
    case LUA_TBOOLEAN:
        writer.Bool(lua_toboolean(L, -1));
        break;
    case LUA_TSTRING: {
        std::string u8str;
        get_lua_value(u8str, L);
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
        int len = luaL_getn(L, -1);
        if (len != 0) {
            writer.StartArray();
            for (int i = 1; i <= len; ++i) {
                lua_rawgeti(L, -1, i);
                get_lua_value_any(writer, L);
                lua_pop(L, 1);
            }
            writer.EndArray(len);
        } else {
            writer.StartObject();
            lua_pushnil(L);
            while (lua_next(L, -2) != 0) {
                std::string u8str;
                // get_lua_u8string(u8str, L, -2);
                get_lua_value(u8str, L, -2);
                writer.Key(u8str);
                get_lua_value_any(writer, L);
                lua_pop(L, 1);
            }
            writer.EndObject();
        }
    } break;
    case LUA_TUSERDATA: {
        FileTime ft;
        writer.StartObject();
        if (OpcUa_IsGood(ft.from_lua(L, -1))) {
            writer.Key("__type").String("FT");
            writer.Key("value").Int64(ft.dt());
        }
        writer.EndObject();
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
template<class Key, class Value>
void set_lua_value_kv(const Key& key, const Value& value, lua_State* L) {
    set_lua_value(key, L);
    set_lua_value(value, L);
    lua_rawset(L, -3);
}
template<class Value>
void get_lua_value_kv(std::string& key, Value& value, lua_State* L) {
    size_t len;
    const char* ch_str = lua_tolstring(L, -2, &len);
    key.assign(ch_str, len);
    get_lua_value(value, L);
    lua_pop(L, 1);
}
template<class Value>
void get_lua_value_kv(int64_t pos, Value& value, lua_State* L) {
    lua_rawgeti(L, -1, pos);
    get_lua_value(value, L);
    lua_pop(L, 1);
}
//
template<class Key, class Value>
void set_lua_value(const std::pair<Key, Value>& kv, lua_State* L) {
    set_lua_value_kv(kv.first, kv.second, L);
}
template<class Key, class Value>
void set_lua_value_kv(size_t pos, const std::pair<Key, Value>& kv, lua_State* L) {
    set_lua_value_kv(kv.first, kv.second, L);
}
template<class Key, class Value>
void get_lua_value(std::pair<Key, Value>& kv, lua_State* L) {
    get_lua_value_kv(kv.first, kv.second, L);
}

template<class It>
void set_lua_value(const It& begin, const It& end, lua_State* L) {
    lua_newtable(L);
    size_t i = 1;
    for (It it = begin; it != end; ++it) {
        set_lua_value_kv(i++, *it, L);
    }
}
template<class It>
void get_lua_value(const It& begin, const It& end, lua_State* L) {
    int len = luaL_getn(L, -1);
    int i = 1;
    for (It it = begin; i <= len && it != end; ++i, ++it) {
        get_lua_value_kv(i, *it, L);
    }
}

struct LuaIntegerType {
    template<class T>
    static void set(const T& value, lua_State* L) {
        lua_pushinteger(L, (lua_Integer)value);
    }
    template<class T>
    static void get(T& value, lua_State* L) {
        value = static_cast<T>(lua_tointeger(L, -1));
    }
};
struct LuaDoubleType {
    template<class T>
    static void set(const T& value, lua_State* L) {
        lua_pushnumber(L, (lua_Number)value);
    }
    template<class T>
    static void get(T& value, lua_State* L) {
        value = lua_tonumber(L, -1);
    }
};

struct LuaIterable {
    template<class T>
    static void set(const T& value, lua_State* L) {
        set_lua_value(value.begin(), value.end(), L);
    }
    template<class T>
    static void get(T& value, lua_State* L) {
        get_lua_value(value.begin(), value.end(), L);
    }
    template<class T>
    static void get(std::vector<T>& arr, lua_State* L) {
        if (!test_lua_value(L, LUA_TTABLE, -1)) {
            return;
        }
        int len = luaL_getn(L, -1);
        arr.resize(len);
        for (int i = 1; i <= len; i++) {
            lua_rawgeti(L, -1, i);
            get_lua_value(arr[i - 1], L);
            lua_pop(L, 1);
        }
    }
    template<class V>
    static void get(std::map<std::string, V>& map, lua_State* L) {
        if (!test_lua_value(L, LUA_TTABLE, -1)) {
            return;
        }
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            std::string key;
            get_lua_value(key, L, -2);
            get_lua_value(map[key], L);
            lua_pop(L, 1);
        }
    }
    template<class T>
    static void set(const std::vector<T>& arr, lua_State* L) {
        lua_createtable(L, 0, arr.size());
        for (size_t i = 0; i < arr.size(); i++) {
            lua_pushnumber(L, i + 1); /* Push the table index */
            set_lua_value(arr[i], L);
            lua_rawset(L, -3); /* Stores the pair in the table */
        }
    }
};
struct LuaBoolVector {
    static void get(std::vector<bool>& arr, lua_State* L) {
        if (!test_lua_value(L, LUA_TTABLE, -1)) {
            return;
        }
        size_t len = lua_rawlen(L, -1);
        arr.resize(len);
        for (size_t i = 1; i <= len; i++) {
            bool val;
            lua_rawgeti(L, -1, i);
            get_lua_value(val, L);
            arr[i - 1] = val;
            lua_pop(L, 1);
        }
    }
    static void set(const std::vector<bool>& arr, lua_State* L) {
        lua_createtable(L, 0, arr.size());
        for (int i = 0; i < arr.size(); i++) {
            lua_pushnumber(L, i + 1);        /* Push the table index */
            set_lua_value((bool)arr[i], L);  // android clang fix
            lua_rawset(L, -3);               /* Stores the pair in the table */
        }
    }
};
template<class RealType>
struct LuaSimpleWrapper {
    template<class T>
    static void set(const T& value, lua_State* L) {
        const RealType tmp = value;
        set_lua_value(tmp, L);
    }
    template<class T>
    static void get(T& value, lua_State* L) {
        RealType tmp;
        get_lua_value(tmp, L);
        value = tmp;
    }
};
template<class T>
struct SelfSetterGetter {
    static void set(const T& value, lua_State* L) {
        value.lua_set(L);
    }
    static void get(T& value, lua_State* L) {
        value.lua_get(L);
    }
};
template<class T>
struct LuaProxyType {
    static void set(const T& value, lua_State* L) {
        value.lua_set(L);
    }
    static void get(T& value, lua_State* L) {
        value.lua_get(L);
    }
};
template<class T>
struct LuaBaseType {
    static void set(const T& value, lua_State* L) {
        BOOST_STATIC_ASSERT_MSG(sizeof(T) == 0, "Not Implemented");
    }
    static void get(T& value, lua_State* L) {
        BOOST_STATIC_ASSERT_MSG(sizeof(T) == 0, "Not Implemented");
    }
    // static const char* name(const T* dummy) { return get_base_lua_type(dummy); }
};

template<class T, class Enable = void>
struct GetBaseLuaType {
    typedef LuaBaseType<T> LuaType;
};
template<>  // Определён одельно так как boost::is_integral<bool> == true
struct GetBaseLuaType<bool> {
    typedef LuaBaseType<bool> LuaType;
};
namespace _ {
    typedef std::vector<bool> mplc_bool_array;
}
template<>  // Определён одельно так как boost::is_integral<bool> == true
struct GetBaseLuaType<_::mplc_bool_array> {
    typedef LuaBoolVector LuaType;
};
template<>  // Определён одельно так как boost::enable_if<is_iterable<T> > == true
struct GetBaseLuaType<std::string, void> {
    typedef LuaBaseType<std::string> LuaType;
};
template<class T>
struct GetBaseLuaType<T, typename boost::enable_if<boost::is_integral<T> >::type> {
    typedef LuaIntegerType LuaType;
};
template<class T>
struct GetBaseLuaType<T, typename boost::enable_if<boost::is_enum<T> >::type> {
    typedef LuaIntegerType LuaType;
};

template<class T>
struct GetBaseLuaType<T, typename boost::enable_if<boost::is_floating_point<T> >::type> {
    typedef LuaDoubleType LuaType;
};

template<class T>
struct GetBaseLuaType<T, typename boost::enable_if<is_simple_wrapper<T> >::type> {
    typedef LuaSimpleWrapper<typename T::lua_type> LuaType;
};
template<class T>
struct GetBaseLuaType<T, typename boost::enable_if<is_iterable<T> >::type> {
    typedef LuaIterable LuaType;
};
template<class T>
struct GetBaseLuaType<T, typename boost::enable_if<is_self_get_set<T> >::type> {
    typedef SelfSetterGetter<T> LuaType;
};
template<class T>
struct GetBaseLuaType<T, typename boost::enable_if<is_lua_proxy_t<T> >::type> {
    typedef LuaProxyType<T> LuaType;
};
template<class T>
void set_lua_value(const T& data, lua_State* L) {
    GetBaseLuaType<T>::LuaType::set(data, L);
}
template<class T>
void get_lua_value(T& data, lua_State* L) {
    GetBaseLuaType<T>::LuaType::get(data, L);
}

namespace mplc { namespace lua {

    struct Encoding {
        enum Type { UTF8, CP1251, ASCII };
        MPLCSHARE_API static std::string Convert(lib::string_view str, Type from, Type into);
    };

}}  // namespace mplc::lua
