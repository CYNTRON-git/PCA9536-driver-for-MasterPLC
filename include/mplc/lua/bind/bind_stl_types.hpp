#pragma once
#include <mplc/libs/containers.hpp>
#include "bind_base.hpp"

namespace mplc { namespace lua {
    namespace detail {
        template<typename, typename = void>
        struct is_iterable : boost::false_type {};
        template<typename T>
        struct is_iterable<T, typename boost::make_void<typename T::const_iterator>::type> : boost::true_type {};
        template<typename T>
        constexpr bool is_iterable_v = is_iterable<T>::value;

        template<typename, typename = void>
        struct has_key_type : boost::false_type {};
        template<typename T>
        struct has_key_type<T, typename boost::make_void<typename T::key_type>::type> : boost::true_type {};
        template<typename T>
        constexpr bool has_key_type_v = has_key_type<T>::value;

        template<typename, typename = void>
        struct has_mapped_type : boost::false_type {};
        template<typename T>
        struct has_mapped_type<T, typename boost::make_void<typename T::mapped_type>::type> : boost::true_type {};
        template<typename T>
        constexpr bool has_mapped_type_v = has_mapped_type<T>::value;

        template<typename, typename = void>
        struct is_kv_container : boost::false_type {};
        template<typename T>
        struct is_kv_container<
            T,
            typename boost::enable_if_c<has_mapped_type_v<T> && has_key_type_v<T> && is_iterable_v<T>>::type>
            : boost::true_type {};

        template<typename, typename = void>
        struct is_val_only : boost::false_type {};
        template<typename T>
        struct is_val_only<
            T,
            typename boost::enable_if_c<!has_mapped_type_v<T> && has_key_type_v<T> && is_iterable_v<T>>::type>
            : boost::true_type {};

    }  // namespace detail

    template<class T>
    struct Bind<T, typename boost::enable_if<detail::is_kv_container<T>>::type> {
        template<class Tag>
        static int push(lua_State* L, const T& container, bool* ok = nullptr) {
            bool r_ok = true;
            lua_createtable(L, 0, container.size());
            for (const auto& elem: container) {
                int count = lua::set<Tag>(L, elem, &r_ok);
                if (!r_ok || count != 2) {
                    lua_pop(L, count);
                    r_ok = false;
                    break;
                }
                lua_rawset(L, -3);
            }
            if (ok)
                *ok = r_ok;
            return 1;
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, T& val, int i = -1) {
            if (lua_type(L, i) != LUA_TTABLE) {
                return OpcUa_BadTypeMismatch;
            }
            lua_pushnil(L);
            while (lua_next(L, -2) != 0) {
                typename T::key_value key;
                lua::get(L, key, -2);
                lua::get(L, val[key], -1);
                lua_pop(L, 1);
            }
            return OpcUa_Good;
        }
    };

    template<class T>
    struct Bind<std::vector<T>> {
        template<class Tag>
        static int push(lua_State* L, const std::vector<T>& arr, bool* ok = nullptr) {
            lua_createtable(L, arr.size(), 0);
            size_t i = 1;
            bool r_ok = true;
            for (const auto& elem: arr) {
                int count = lua::set(L, i, &r_ok);
                if (r_ok) {
                    count += lua::set<Tag, T>(L, elem, &r_ok);
                }
                if (!r_ok || count != 2) {
                    lua_pop(L, count);
                    break;
                }
                lua_rawset(L, -3);
                ++i;
            }
            if (!r_ok) {
                lua_pop(L, 1);
            }
            if (ok)
                *ok = r_ok;
            return 1;
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, std::vector<T>& arr, int idx = -1) {
            if (lua_type(L, idx) != LUA_TTABLE) {
                return OpcUa_BadTypeMismatch;
            }
            size_t len = lua_rawlen(L, idx);
            arr.resize(len);
            for (size_t i = 0; i < len; i++) {
                lua_rawgeti(L, idx, i + 1);
                lua::get<Tag>(L, arr[i]);
                lua_pop(L, 1);
            }
            return OpcUa_Good;
        }
    };
    template<>
    struct Bind<std::vector<bool>> {
        template<class Tag>
        static int push(lua_State* L, const std::vector<bool>& arr, bool* ok = nullptr) {
            lua_createtable(L, arr.size(), 0);
            size_t i = 1;
            for (auto elem: arr) {
                lua_pushinteger(L, i);
                lua_pushboolean(L, elem);
                lua_rawset(L, -3);
            }
            if (ok) {
                *ok = true;
            }
            return 1;
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, std::vector<bool>& arr, int idx = -1) {
            if (lua_type(L, idx) != LUA_TTABLE) {
                return OpcUa_BadTypeMismatch;
            }
            size_t len = lua_rawlen(L, idx);
            arr.resize(len);
            for (size_t i = 0; i < len; i++) {
                lua_rawgeti(L, idx, i + 1);
                arr[i] = lua_toboolean(L, -1);
                lua_pop(L, 1);
            }
            return OpcUa_Good;
        }
    };
}}  // namespace mplc::lua
