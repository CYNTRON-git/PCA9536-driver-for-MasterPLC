#pragma once
#include "../bind_base.hpp"
#include "../bind_filetime.hpp"
#include "../bind_json.hpp"
//#include "mplc/lua/lua_writer.h"
#include "share/opcua_variant_hlp.h"
#include "share/lua/lua_addins.h"

namespace mplc { namespace lua {
    namespace detail {
        /*template<class T>
        struct OpcUaArrayWrapper {
            T*& data;
            OpcUa_Int32& size;
            OpcUaArrayWrapper(T*& ptr, OpcUa_Int32& len): data(ptr), size(len) {}
        };
        template<class T>
        OpcUaArrayWrapper<T> opc_array_wrapper(T*& ptr, OpcUa_Int32& len) {
            return OpcUaArrayWrapper<T>(ptr, len);
        }*/

        template<class T, OpcUa_BuiltInType SubType>
        class type_wrap {};
    }  // namespace detail

    template<class Tag, OpcUa_BuiltInType SubType, class T>
    int set(lua_State* L, const T& val, bool* ok = nullptr) {
        return Bind<detail::type_wrap<T, SubType>>::template push<Tag>(L, val, ok);
    }
    template<class Tag, OpcUa_BuiltInType SubType, class T>
    int set(lua_State* L, const T* data, OpcUa_Int32 size, bool* ok = nullptr) {
        lua_createtable(L, size, 0);
        bool is_ok = true;
        for (OpcUa_Int32 i = 0; i < size; ++i) {
            lua_pushinteger(L, i + 1);
            int count = lua::set<Tag, SubType>(L, data[i], &is_ok);
            if (count != 1 || !is_ok) {
                lua_pop(L, count + 1);
                continue;
            }
            lua_rawset(L, -3);
        }
        return 1;
    }
    template<class Tag, OpcUa_BuiltInType SubType, class T>
    OpcUa_StatusCode get(lua_State* L, T& val, int idx = -1) {
        return Bind<detail::type_wrap<T, SubType>>::template get<Tag>(L, val, idx);
    }
    template<class Tag, OpcUa_BuiltInType SubType, class T>
    static OpcUa_StatusCode get(lua_State* L, T*& data, OpcUa_Int32& size, int idx = -1) {
        if (lua_type(L, idx) != LUA_TTABLE) {
            // TODO: PRINT ERROR!
            return OpcUa_BadTypeMismatch;
        }
        size_t len = lua_rawlen(L, idx);
        if (len > 0) {
            data = (T*)OpcUa_Alloc(len * sizeof(T));
            size = len;
            for (size_t i = 0; i < len; i++) {
                lua_rawgeti(L, idx, i + 1);
                lua::get<Tag, SubType>(L, data[i]);
                lua_pop(L, 1);
            }
        }
        return OpcUa_Good;
    }

    // template<class T, OpcUa_BuiltInType SubType>
    // struct Bind<detail::type_wrap<detail::OpcUaArrayWrapper<T>, SubType>> {
    //     template<class Tag>
    //     static int push(lua_State* L, const detail::OpcUaArrayWrapper<T>& val, bool* ok = nullptr) {
    //         lua_createtable(L, val.size, 0);
    //         bool is_ok = true;
    //         for (OpcUa_Int32 i = 0; i < val.size; ++i) {
    //             lua_pushinteger(L, i + 1);
    //             int count = lua::set<Tag, SubType>(L, val.data[i], &is_ok);
    //             if (count != 1 || !is_ok) {
    //                 lua_pop(L, count + 1);
    //                 continue;
    //             }
    //             lua_rawset(L, -3);
    //         }
    //         return 1;
    //     }
    //     template<class Tag>
    //     static OpcUa_StatusCode get(lua_State* L, detail::OpcUaArrayWrapper<T>& val, int idx = -1) {
    //         if (lua_type(L, idx) != LUA_TTABLE) {
    //             // TODO: PRINT ERROR!
    //             return OpcUa_BadTypeMismatch;
    //         }
    //         size_t len = lua_rawlen(L, idx);
    //         if (len > 0) {
    //             val.data = (T*)OpcUa_Alloc(len * sizeof(T));
    //             val.size = len;
    //             for (size_t i = 0; i < len; i++) {
    //                 lua_rawgeti(L, idx, i + 1);
    //                 lua::get<Tag, SubType>(L, val.data[i]);
    //                 lua_pop(L, 1);
    //             }
    //         }
    //         return OpcUa_Good;
    //     }
    // };

    template<class T, OpcUa_BuiltInType SubType>
    struct Bind<detail::type_wrap<T, SubType>> {
        template<class Tag>
        static int push(lua_State* L, const T& val, bool* ok = nullptr) {
            return lua::set<Tag>(L, val, ok);
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, T& val, int idx = -1) {
            return lua::get<Tag>(L, val, idx);
        }
    };
}}  // namespace mplc::lua
