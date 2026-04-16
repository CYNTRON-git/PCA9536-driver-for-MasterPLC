#pragma once
#define BOOST_PP_VARIADICS 1
#include <boost/preprocessor.hpp>

/* MPLC_PP_CAT */
#if ~BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_MWCC()
#    define MPLC_PP_CAT(a, b) MPLC_PP_CAT_I(a, b)
#else
#    define MPLC_PP_CAT(a, b) MPLC_PP_CAT_OO((a, b))
#    define MPLC_PP_CAT_OO(par) MPLC_PP_CAT_I##par
#endif
#
#if (~BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_MSVC()) || (defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 1700)
#    define MPLC_PP_CAT_I(a, b) a##b
#else
#    define MPLC_PP_CAT_I(a, b) MPLC_PP_CAT_II(~, a##b)
#    define MPLC_PP_CAT_II(p, res) res
#endif
/* MPLC_PP_CAT_RESERV */
#if ~BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_MWCC()
#    define MPLC_PP_CAT_RESERV(a, b) MPLC_PP_CAT_RESERV_I(a, b)
#else
#    define MPLC_PP_CAT_RESERV(a, b) MPLC_PP_CAT_RESERV_OO((a, b))
#    define MPLC_PP_CAT_RESERV_OO(par) MPLC_PP_CAT_RESERV_I##par
#endif
#
#if (~BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_MSVC()) || (defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 1700)
#    define MPLC_PP_CAT_RESERV_I(a, b) a##b
#else
#    define MPLC_PP_CAT_RESERV_I(a, b) MPLC_PP_CAT_RESERV_II(~, a##b)
#    define MPLC_PP_CAT_RESERV_II(p, res) res
#endif

#define MPLC_PP_EMPTY(...)
#define MPLC_PP_EXPAND(x) x
#define MPLC_PP_CAT_N(Prefix, ...) MPLC_PP_CAT_RESERV(Prefix, BOOST_PP_VARIADIC_SIZE(__VA_ARGS__))

// --------------------------------
#define MPLC_PP_SET_LUA_VALUE(z, n, text) set_lua_value(text##n, L);

#define MPLC_PP_EXPAND_TEMPLATE(Count, text) template<BOOST_PP_ENUM_PARAMS(Count, text)>

#define MPLC_PP_GET_LUA_VALUE(z, n, Max)                                                                               \
    get_lua_value(BOOST_PP_CAT(v, BOOST_PP_SUB(Max, n)), L);                                                           \
    lua_pop(L, 1);

#define MPLC_PP_GENERATE_POP_LUA_RESULT(z, TCount, _)                                                                  \
    BOOST_PP_IF(TCount, MPLC_PP_EXPAND_TEMPLATE, MPLC_PP_EMPTY)                                                        \
    (TCount, class T) int pop(BOOST_PP_ENUM_BINARY_PARAMS(TCount, T, &v)) {                                            \
        int ret_v = lua_gettop(L);                                                                                     \
        if (TCount > ret_v - count - 1) {                                                                              \
            return 1;                                                                                                  \
        }                                                                                                              \
        BOOST_PP_REPEAT(TCount, MPLC_PP_GET_LUA_VALUE, BOOST_PP_DEC(TCount))                                           \
        return 0;                                                                                                      \
    }

#define MPLC_PP_GENERATE_LUA_EXECUTOR(z, PCount, FName)                                                                \
    BOOST_PP_IF(PCount, MPLC_PP_EXPAND_TEMPLATE, MPLC_PP_EMPTY)                                                        \
    (PCount, class T)                                                                                                  \
                                                                                                                       \
        inline int                                                                                                     \
        FName(lua_State* L BOOST_PP_COMMA_IF(PCount) BOOST_PP_ENUM_BINARY_PARAMS(PCount, const T, &v)) {               \
        BOOST_PP_REPEAT(PCount, MPLC_PP_SET_LUA_VALUE, v)                                                              \
        int status = lua_pcall(L, PCount, LUA_MULTRET, 0);                                                             \
        if (status) {                                                                                                  \
            PRINTLN("%s", lua_tostring(L, -1));                                                                        \
            lua_pop(L, 1);                                                                                             \
        }                                                                                                              \
        return status;                                                                                                 \
        /*int res = lua_tointeger(L, -1);                                                                              \
        lua_pop(L, 1);                                                                                                 \
        return res;*/                                                                                                  \
    }

#define MPLC_PP_GENERATE_LUA_EXECUTOR_BIND(z, PCount, FName)                                                           \
    BOOST_PP_IF(PCount, MPLC_PP_EXPAND_TEMPLATE, MPLC_PP_EMPTY)                                                        \
    (PCount, class T) int operator()(BOOST_PP_ENUM_BINARY_PARAMS(PCount, const T, &v)) {                               \
        int prev_res_count = lua_gettop(L) - count - 1;                                                                \
        if (prev_res_count > 0) {                                                                                      \
            lua_pop(L, prev_res_count);                                                                                \
        } else if (prev_res_count < 0) {                                                                               \
            bad_stack();                                                                                               \
        }                                                                                                              \
        lua_pushvalue(L, -1); /*func */                                                                                \
        return FName(L BOOST_PP_COMMA_IF(PCount) BOOST_PP_ENUM_PARAMS(PCount, v));                                     \
    }
