#pragma once
#include <mplc/libs/containers.hpp>
#include "bind_base.hpp"
#include <boost/type_traits.hpp>

namespace mplc { namespace lua {
#define _PP_LUA_GET_VALUE(z, n, Pefix)                                                                                 \
    BOOST_PP_CAT(Pefix, n) BOOST_PP_CAT(arg, n);                                                                       \
    lua::get<Tag>(L, BOOST_PP_CAT(arg, n), BOOST_PP_INC(n));

#define _PP_LUA_TYPEDEF(z, n, Pefix)                                                                                   \
    typedef typename clear_type<BOOST_PP_CAT(Pefix, n)>::type BOOST_PP_CAT(c, BOOST_PP_CAT(Pefix, n));

#define _PP_WRAPPER(z, N, StructName)                                                                                  \
    template<class Tag, class Ret BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, class Arg)>                             \
    struct BOOST_PP_CAT(StructName, N) {                                                                               \
        BOOST_PP_REPEAT(N, _PP_LUA_TYPEDEF, Arg)                                                                       \
        typedef Ret (*function)(BOOST_PP_ENUM_PARAMS(N, Arg));                                                         \
        static int call(lua_State* L) {                                                                                \
            return __call(L, clear_type<Ret>::dump());                                                                 \
        }                                                                                                              \
        static int __call(lua_State* L, const void*) {                                                                 \
            BOOST_PP_REPEAT(N, _PP_LUA_GET_VALUE, cArg)                                                                \
            function fun = static_cast<function>(lua_touserdata(L, lua_upvalueindex(1)));                              \
            fun(BOOST_PP_ENUM_PARAMS(N, arg));                                                                         \
            return 0;                                                                                                  \
        }                                                                                                              \
        template<class RetV>                                                                                           \
        static int __call(lua_State* L, const RetV*) {                                                                 \
            BOOST_PP_REPEAT(N, _PP_LUA_GET_VALUE, cArg)                                                                \
            function fun = static_cast<function>(lua_touserdata(L, lua_upvalueindex(1)));                              \
            return lua::set<Tag>(L, fun(BOOST_PP_ENUM_PARAMS(N, arg)));                                                \
            ;                                                                                                          \
        }                                                                                                              \
    };                                                                                                                 \
    template<class Tag, class Ret BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, class Arg)>                             \
    static int create_bind(lua_State* L, Ret (*Fun)(BOOST_PP_ENUM_PARAMS(N, Arg))) {                                   \
        lua_pushlightuserdata(L, Fun);                                                                                 \
        lua_pushcclosure(L,                                                                                            \
                         BOOST_PP_CAT(StructName, N) < Tag,                                                            \
                         Ret BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, Arg) > ::call,                               \
                         1);                                                                                           \
        return 1;                                                                                                      \
    }

    namespace detail {
        template<class Type>
        struct clear_type {
            typedef boost::remove_cv_ref_t<Type> type;
            static const type* dump() {
                return nullptr;
            }
        };

        BOOST_PP_REPEAT(10, _PP_WRAPPER, wrapper);
        template<class Tag>
        static int create_bind(lua_State* L, int (*Fun)(lua_State*)) {
            lua_pushcclosure(L, Fun, 0);
            return 1;
        }

    }  // namespace detail

    template<class T>
    struct Bind<T, typename boost::enable_if<boost::is_function<boost::remove_pointer_t<T>>>::type> {
        template<class Tag>
        static int push(lua_State* L, T func, bool* ok = nullptr) {
            if (ok)
                *ok = true;
            return detail::create_bind<Tag>(L, func);
        }
        template<class Tag>
        static void get(lua_State* L, T& val, int i = -1) {
            lua_getupvalue(L, i, 1);
            val = static_cast<T>(lua_touserdata(L, -1));
            lua_pop(L, 1);
        }
    };

#undef _PP_LUA_GET_VALUE
#undef _PP_LUA_TYPEDEF
#undef _PP_WRAPPER
}}  // namespace mplc::lua
