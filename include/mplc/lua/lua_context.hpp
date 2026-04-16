#pragma once
#include <lua.hpp>
#include <string>
#include <mplc/msgpack_ext.hpp>
#include "lua_bind.hpp"
#include <mplc/lua/lua_function.h>
//#include "lua_reg_ud.h"
//#define luaL_newlibtable(L, l) lua_createtable(L, 0, sizeof(l) / sizeof((l)[0]) - 1)
//#define lua_pcallk(lua_State* L, int nargs, int nresults, int msgh, lua_KContext ctx, lua_KFunction k);

#ifndef LUA_KCONTEXT
typedef void* lua_KContext;
typedef int (*lua_KFunction)(lua_State* L, int status, lua_KContext ctx);
inline int lua_pcallk(lua_State* L, int nargs, int nresults, int msgh, lua_KContext ctx, lua_KFunction k) {
    return lua_pcall(L, nargs, nresults, msgh);
}
#endif
namespace mplc { namespace lua {

    class LuaContext {
        bool m_clear = true;

    public:
        explicit LuaContext(bool openDefaultLibs = true) {
            // luaL_newstate can return null if allocation failed
            L = luaL_newstate();
            if (L == nullptr)
                throw std::bad_alloc();

            // setting the panic function
            lua_atpanic(L, lua_panic);

            // opening default library if required to do so
            if (openDefaultLibs) {
                luaL_openlibs(L);
                luaL_openSTlibs(L);
            }
        }
        explicit LuaContext(lua_State* state, bool clear = true): m_clear(clear) {
            // luaL_newstate can return null if allocation failed
            L = state;
        }
        static int lua_panic(lua_State* L) {
            const std::string str = lua_tostring(L, -1);
            lua_pop(L, 1);
            printf("lua_atpanic triggered\n");
            return 0;
        }
        LuaContext(const LuaContext&) = delete;
        LuaContext& operator=(const LuaContext&) = delete;
        ~LuaContext() noexcept {
            if (m_clear)
                lua_close(L);
        }

        /*template<class Type>
        int push(const Type& val, bool* ok = nullptr) {
            return Binder<Type>::template push<void>(m_state, val, ok);
        }
        template<class Tag, class Type>
        int push(const Type& val, bool* ok = nullptr) {
            return Binder<Type>::template push<Tag>(m_state, val, ok);
        }*/
        // int run() { return lua_pcall(L, 0, LUA_MULTRET, 0); }
        const char* error() {
            return lua_tostring(L, -1);
        }
        template<class Tag, class T>
        bool _set(const T& val) {
            bool ok = false;
            lua::set<Tag>(L, val, &ok);
            return ok;
        }

#ifndef BOOST_HAS_VARIADIC_TMPL
#    define __PP_LUA_CALL_SET(z, n, data)                                                                              \
        count += lua::set<Tag>(L, BOOST_PP_CAT(data, n), &ok);                                                         \
        if (!ok) {                                                                                                     \
            lua_settop(L, top);                                                                                        \
            return -1;                                                                                                 \
        }

#    define __PP_LUA_CALL(z, n, name)                                                                                  \
        template<class Tag BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, class Arg)>                                    \
        int name(const char* name BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_BINARY_PARAMS(n, Arg, arg)) {                     \
            int top = lua_gettop(L);                                                                                   \
            lua_getglobal(L, name);                                                                                    \
            if (lua_type(L, -1) == LUA_TNIL) {                                                                         \
                lua_pushfstring(L, "Global var function %s is absent", name);                                          \
                return -1;                                                                                             \
            }                                                                                                          \
            bool ok = false;                                                                                           \
            int count = 0;                                                                                             \
            BOOST_PP_REPEAT(n, __PP_LUA_CALL_SET, arg)                                                                 \
            return lua_pcallk(L, count, LUA_MULTRET, 0, 0, NULL);                                                      \
        }
        BOOST_PP_REPEAT(10, __PP_LUA_CALL, pcall)

#    undef __PP_LUA_CALL_SET
#    undef __PP_LUA_CALL
        int pcall(const char* name) {
            if (== LUA_TNIL) {
                lua_pushfstring(L, "Global var function %s is absent", name);
                lua_remove(L, -2);
                return -1;
            }
            lua_getglobal(L, name);
            return lua_pcallk(L, 0, LUA_MULTRET, 0, 0, NULL);
        }
#else
        template<class... Args>
        int pcall(const char* name, Args&&... args) {
            lua::lua_function func(L, name);
            return func(std::forward<Args>(args)...);
        }
#endif

        template<class Type>
        void setGlobal(const char* name, const Type& val, bool* ok = nullptr) {
            setGlobal<void>(name, val, ok);
        }
        template<class Tag, class Type>
        void setGlobal(const char* name, const Type& val, bool* ok = nullptr) {
            int n = lua::set<Tag>(L, val, ok);
            if (n == 0) {
                lua_pushnil(L);
            } else if (n > 1) {
                lua_pop(L, n - 1);
            }
            lua_setglobal(L, name);
        }
        lua_function function(lib::string_view name) {
            return lua_function(L, name);
        }
        template<class Type>
        int push(const Type& val, bool* ok = nullptr) {
            return lua::set<void>(L, val, ok);
        }
        void pushNil() {
            lua_pushnil(L);
        }
        template<class Tag, class Type>
        int push(const Type& val, bool* ok = nullptr) {
            return lua::set<Tag>(L, val, ok);
        }

        template<class Type>
        void getGlobal(Type& val, const char* name) {
            getGlobal<void>(name, val);
        }
        template<class Tag, class Type>
        void getGlobal(const char* name, Type& val) {
            lua_getglobal(L, name);

            lua::get<Tag>(L, val, -1);
            lua_pop(L, 1);
        }
        template<class Tag, class Type>
        Type getGlobal(const char* name) {
            lua_getglobal(L, name);
            Type v = get<Tag, Type>(-1);
            lua_pop(L, 1);
            return v;
        }
        template<class Type>
        Type getGlobal(const char* name) {
            return getGlobal<void, Type>(name);
        }
        template<class Type>
        void getStack(Type& val, int i = -1) {
            lua::get<void>(L, val, i);
        }
        template<class Tag, class Type>
        void getStack(Type& val, int i = -1) {
            lua::get<Tag>(L, val, i);
        }
        template<class Tag, class Type>
        void getField(Type& val, const char* field, int idx = -1) {
            lua_getfield(L, idx, field);
            lua::get<Tag>(L, val, idx);
        }
        template<class Type>
        void getField(Type& val, const char* field, int idx = -1) {
            getField<void, Type>(val, field, idx);
        }
        template<class Type>
        Type getField(const char* field, int idx = -1) {
            Type val;
            getField<void, Type>(val, field, idx);
            return val;
        }
        template<class Type>
        Type get(const char* name) {
            return get<void>(name);
        }

        template<class Type>
        Type get(int i = -1) {
            return get<void, Type>(i);
        }
        int getType(int i = -1) const {
            return lua_type(L, i);
        }
        int getTop() const {
            return lua_gettop(L);
        }
        void setTop(int i) const {
            lua_settop(L, i);
        }
        template<class Tag, class Type>
        Type get(int i = -1) {
            Type val;
            lua::get<Tag>(L, val, i);
            return val;
        }
        const char* getStr(int idx = -1, size_t* size = nullptr) const {
            return lua_tolstring(L, idx, size);
        }
        void pop(int n = 1) {
            lua_pop(L, n);
        }
        void regCLib(const char* name, const luaL_Reg* lib) {
            lua_createtable(L, 0, 0);
            luaL_setfuncs(L, lib, 0);
            lua_setglobal(L, name);
        }
        int f_load(const std::string& str) {
            return luaL_loadfile(L, str.c_str()) || lua_pcall(L, 0, LUA_MULTRET, 0);
        }
        bool addLibFuncs(const char* name, const luaL_Reg lib[]) {
            lua_getglobal(L, name);
            if (lua_isnil(L, -1)) {
                lua_pop(L, -1);
                return false;
            }
            luaL_setfuncs(L, lib, 0);
            lua_setglobal(L, name);
            return true;
        }

        void newMeta(const char* name, const luaL_Reg meta[]) {
            if (luaL_newmetatable(L, name)) {
                luaL_setfuncs(L, meta, 0);
                lua_pop(L, 1);
            }
        }
        int load_chunk(lib::string_view chank, const char* name) {
            return luaL_loadbufferx(L, chank.data(), chank.size(), name, "t") || lua_pcall(L, 0, LUA_MULTRET, 0);
        }

        // template<class T, int N>
        // void regType(const char* name, lua::IFieldMeta (&fields)[N]) {
        //     UserData<T>::RegFBType(L, name, fields, N);
        // }

    public:
        lua_State* L;
    };  // namespace lua

    // template<class Tag, class Type, class Field>
    // struct StructField : IField {
    //    Field Type::*param;
    //    StructField(Field Type::*param, bool retain): IField(retain), param(param) {}
    //    int set(const void* ptr, lua_State* L, bool* ok = nullptr) const override {
    //        const Type* obj = static_cast<const Type*>(ptr);
    //        return lua::set<Tag>(L, obj->*param, ok);
    //    }
    //    void get(void* ptr, lua_State* L, int i = -1) const override {
    //        Type* obj = static_cast<Type*>(ptr);
    //        get_lua_value(obj->*param, L);
    //        lua::get<Tag>(L, obj->*param, i);
    //    }
    //};
    // template<class Tag, class Type, class Field>
    // IField* IField::create(Field Type::*field, bool retain) {
    //    return new StructField<Tag, Type, Field>(field, retain);
    //}
}}  // namespace mplc::lua
