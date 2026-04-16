#pragma once
#include <lua.hpp>
#include <string>
#include <typeinfo>
#include <mplc/libs/containers.hpp>
#include "lua_context.hpp"
#include "bind/bind_string.hpp"

namespace mplc { namespace lua {

    template<class Type>
    struct UserData {
        typedef lib::unordered_map<lib::string_view, FieldMeta> FieldsMap;
        typedef lib::unordered_map<std::string, lua_CFunction> ExtendMetaTable;
        static ExtendMetaTable metatable;
        static FieldsMap fields;
        static std::string name;
        static int InitFields(lua_State* L, Type* obj) {
            lua_pushnil(L);
            // stack now contains: -1 => nil; -2 => table
            while (lua_next(L, -2)) {
                if (lua_isstring(L, -2)) {
                    size_t len;
                    const char* key = lua_tolstring(L, -2, &len);
                    auto it = fields.find(key);
                    if (it != fields.end()) {
                        it->second.getter(L, obj, -1);
                    }
                }
                lua_pop(L, 1);
            }
            return 0;
        }
        static Type* GetUserObject(lua_State* L, int i = 1) {
            static const std::string error = std::string(LuaName()) + std::string("expected.");
            void* ud = luaL_checkudata(L, i, LuaName());
            luaL_argcheck(L, ud != NULL, i, error.c_str());
            return static_cast<Type*>(ud);
        }
        static int New(lua_State* L) {
            if (L == nullptr)
                return 0;
            Type* obj = new (lua_newuserdata(L, sizeof(Type))) Type();
            luaL_getmetatable(L, LuaName());
            lua_setmetatable(L, -2);
            lua_pushvalue(L, -2);
            InitFields(L, obj);
            lua_pop(L, 1);
            // obj->Init(L);
            return 1;
        }
        /*static void set(const Type& value, lua_State* L) { fields.WriteAllTo(&value, L); }
        static void get(Type& value, lua_State* L) { fields.ReadAllFrom(&value, L); }*/
        static int __call(lua_State* L) {
            // return GetUserObject(L)->call(L);
            return 0;
        }

        static int __eq(lua_State* L) {
            Type* obj1 = GetUserObject(L, 1);
            Type* obj2 = GetUserObject(L, 2);
            if (!obj1 || !obj2)
                return lua::set(L, false);
            bool eq = true;
            for (auto it = fields.begin(); it != fields.end() && eq; ++it) {
                bool ok = false;
                int n = it->second.set(L, obj1, &ok);
                int k = it->second.set(L, obj2, &ok);
                if (!ok || n != k) {
                    lua_pop(L, n + k);
                    return lua::set(L, false);
                }
                for (int i = 1; i < n + 1 && eq; ++i) {
                    eq = lua_compare(L, i, n + i, LUA_OPEQ) == 1;
                }
                lua_pop(L, n + k);
            }

            return lua::set(L, eq);
        }
        static int serialize_to_lua(lua_State* L) {
            Type* obj = GetUserObject(L);
            if (!obj)
                return 0;
            lua_createtable(L, 0, fields.size());
            for (const auto& it: fields) {
                lua::set(L, it.first);
                bool ok = false;
                int n = it.second.set(L, obj, &ok);
                if (!ok)
                    lua_pop(L, n);
                if (n > 1)
                    lua_pop(L, n - 1);
                lua_settable(L, -3);
            }
            // lua_pushboolean(L, true);
            // if (obj->check()) return 1;
            // detail::ObjLockGuard<Type> lock(obj);
            return 1;  // fields.WriteAllTo(obj, L);
        }

        static lua_CFunction IsFunction(const char* name) {
            static const luaL_Reg Lib_f[] = {{"__serialize_to_lua", serialize_to_lua}, {nullptr, nullptr}};
            const luaL_Reg* fn = Lib_f;
            while (fn->func != nullptr) {
                if (strcmp(name, fn->name) == 0)
                    return fn->func;
                ++fn;
            }
            return nullptr;
        }
        static int __newindex(lua_State* L) {
            Type* obj = GetUserObject(L);
            return fields.ReadFrom(obj, L);
        }
        static int __index(lua_State* L) {
            Type* obj = GetUserObject(L);
            if (lua_isstring(L, -1)) {
                const char* name = lua_tostring(L, -1);
                lua_CFunction fn = IsFunction(name);
                if (fn) {
                    lua_pushcclosure(L, fn, 0);
                    return 1;
                }
            }
            // detail::ObjLockGuard<Type> lock(obj);
            return 0;  // fields.WriteTo(obj, L);
        }
        static int __gc(lua_State* L) {
            GetUserObject(L)->~Type();
            return 0;
        }
        // static void RegSTLib() { Type::RegAsLuaType(); }
        static const char* LuaName() {
            return name.c_str();
        }

        static void BindFields() {
            // detail::BindFields<Type, Type>::bind();
            // HasDynamicFields<Type>::bind();
        }
        static int RegFBType(lua_State* L, const char* lib_name, lua::IFieldMeta* params, size_t N) {
            static const luaL_Reg Lib_f[] = {{"new", New},
                                             {"get", __index},
                                             {"set", __newindex},
                                             {"call", __call},
                                             {nullptr, nullptr}};

            static const luaL_Reg Lib_m[] = {{"__index", __index},
                                             {"__newindex", __newindex},
                                             {"__call", __call},
                                             {"__eq", __eq},
                                             {"__gc", __gc},
                                             {nullptr, nullptr}};
            name = lib_name;
            for (size_t i = 0; i < N; ++i) {
                /*lua::IFieldMeta* f = new FieldMeta(params[i]);
                auto it = fields.find(f->name);
                if (it != fields.end()) {
                    FieldMeta* tmp = it->second;
                    fields.erase(it);
                    delete tmp;
                }
                fields[f->name] = f;*/
            }
            BindFields();
            RegCustomType(L, Lib_m, Lib_f);
            return 1;
        }

        static int RegCustomType(lua_State* L, const luaL_Reg* Lib_m, const luaL_Reg* Lib_f) {
            luaL_newmetatable(L, LuaName());
            luaL_setfuncs(L, Lib_m, 0);
            lua_pop(L, 1);
            if (Lib_f) {
                lua_createtable(L, 0, 0);
                luaL_setfuncs(L, Lib_f, 0);
                lua_setglobal(L, LuaName());
            }
            return 1;
        }
    };
    // template<class Tag, class Type, class FieldType, FieldType Type::*param>
    // static FieldMeta bind_field(const char* name, bool retain) {
    //     // FieldMeta f(name,
    //     //             FieldMeta::get<Tag, Type, FieldType, param>,
    //     //             FieldMeta::set<Tag, Type, FieldType, param>,
    //     //             retain);
    //     return FieldMeta<Tag, Type, FieldType, param>(name);
    // }

    template<class Type>
    typename UserData<Type>::FieldsMap UserData<Type>::fields;
    template<class Type>
    typename UserData<Type>::ExtendMetaTable UserData<Type>::metatable;
    template<class Type>
    std::string UserData<Type>::name;
}}  // namespace mplc::lua
