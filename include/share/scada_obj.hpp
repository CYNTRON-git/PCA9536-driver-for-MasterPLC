#pragma once
#include "scada_fields.h"

namespace SCADA_API {
    namespace detail {

        template<class _Cur, class _N = void>
        struct TypeList {
            typedef _Cur type;
            typedef _N next;
        };

        template<class Child, class Parent, class R = void>
        struct BindFields {
            static void bind() {
                Parent::template BindFields<Child>();
            }
        };
        template<class Child>
        struct BindFields<Child, void, void> {
            static void bind() {}
        };

        template<class T>
        struct ObjLockGuard;

    }  // namespace detail
    template<class T, class R = void>
    struct HasDynamicFields {
        static void bind() {}
    };

    template<class UserType>
    struct ScadaObj {
        static ScadaFields fields;
        static UserType* GetUserObject(lua_State* L) {
            static const std::string error = std::string(LuaName()) + std::string("expected.");
            // return static_cast<UserType*>(lua_touserdata(L, 1));
            void* ud = luaL_checkudata(L, 1, LuaName());
            luaL_argcheck(L, ud != NULL, 1, error.c_str());
            return static_cast<UserType*>(ud);
        }
        static int New(lua_State* L) {
            if (L == nullptr)
                return 0;
            UserType* obj = new (lua_newuserdata(L, sizeof(UserType))) UserType();
            luaL_getmetatable(L, LuaName());
            lua_setmetatable(L, -2);
            lua_pushvalue(L, -2);
            fields.ReadAllFrom(obj, L);
            lua_pop(L, 1);
            obj->Init(L);
            return 1;
        }
        static UserType* NewObj(lua_State* L) {
            if (L == nullptr)
                return nullptr;
            UserType* obj = new (lua_newuserdata(L, sizeof(UserType))) UserType();
            luaL_getmetatable(L, LuaName());
            lua_setmetatable(L, -2);
            lua_pushvalue(L, -2);
            fields.ReadAllFrom(obj, L);
            lua_pop(L, 1);
            obj->Init(L);
            return obj;
        }
        static void set(const UserType& value, lua_State* L) {
            fields.WriteAllTo(&value, L);
        }
        static void get(UserType& value, lua_State* L) {
            fields.ReadAllFrom(&value, L);
        }
        static int Call(lua_State* L) {
            return GetUserObject(L)->call(L);
        }
        static int Clone(lua_State* L) {
            UserType* obj = GetUserObject(L);
            // if (obj->check()) return 1;
            detail::ObjLockGuard<UserType> lock(obj);
            return fields.WriteAllTo(obj, L);
        }
        static int Clone(lua_State* L, const UserType& obj) {
            // if (obj->check()) return 1;
            detail::ObjLockGuard<UserType> lock(&obj);
            return fields.WriteAllTo(&obj, L);
        }
        static lua_CFunction IsFunction(const char* name) {
            static const luaL_Reg Lib_f[] = {{"__clone", Clone}, {nullptr, nullptr}};
            const luaL_Reg* fn = Lib_f;
            while (fn->func != nullptr) {
                if (strcmp(name, fn->name) == 0)
                    return fn->func;
                ++fn;
            }
            return nullptr;
        }
        static int GetField(lua_State* L, UserType& obj) {
            detail::ObjLockGuard<UserType> lock(&obj);
            return fields.ReadFrom(&obj, L);
        }
        static int SetField(lua_State* L, const UserType& obj) {
            if (lua_isstring(L, -1)) {
                const char* name = lua_tostring(L, -1);
                lua_CFunction fn = IsFunction(name);
                if (fn) {
                    lua_pushcclosure(L, fn, 0);
                    return 1;
                }
            }
            detail::ObjLockGuard<UserType> lock(&obj);
            return fields.WriteTo(&obj, L);
        }
        static int Set(lua_State* L) {
            size_t len;
            auto str = lua_tolstring(L, -2, &len);
            mplc::lib::string_view key{str, len};
            if (key == "EN" || key == "EnO" || key == "__Internal" || key == "__VarValues") {
                return 0;
            }
            UserType* obj = GetUserObject(L);
            if (obj->check(L, -2))
                return 0;
            detail::ObjLockGuard<UserType> lock(obj);
            return fields.ReadFrom(obj, L);
        }
        static int Get(lua_State* L) {
            size_t len;
            auto str = lua_tolstring(L, -1, &len);
            mplc::lib::string_view key{str, len};
            if (key == "__Internal") {
                lua_pushboolean(L, true);
                return 1;
            }
            UserType* obj = GetUserObject(L);
            if (key == "EnO") {
                lua_pushboolean(L, obj->data.EnO);
                return 1;
            }
            if (key == "EN") {
                lua_pushboolean(L, obj->data.EN);
                return 1;
            }
            if (key == "__VarValues") {
                if (obj->data.VarValuesRef == LUA_NOREF) {
                    lua_newtable(L);
                    obj->data.VarValuesRef = luaL_ref(L, LUA_REGISTRYINDEX);
                }
                lua_rawgeti(L, LUA_REGISTRYINDEX, obj->data.VarValuesRef);
                return 1;
            }
            if (lua_isstring(L, -1)) {
                const char* name = lua_tostring(L, -1);
                lua_CFunction fn = IsFunction(name);
                if (fn) {
                    lua_pushcclosure(L, fn, 0);
                    return 1;
                }
            }
            detail::ObjLockGuard<UserType> lock(obj);
            return fields.WriteTo(obj, L);
        }
        static int GC(lua_State* L) {
            GetUserObject(L)->~UserType();
            return 1;
        }
        static void RegSTLib() {
            UserType::RegAsLuaType();
        }
        static const char* LuaFullName() {
            return UserType::_FullName();
        }
        static const char* LuaName() {
            return UserType::_ShortName();
        }
        static std::string MetaTable() {
            const ScadaFields::LuaMeta& meta = fields.meta;
            std::ostringstream oss;
            oss << std::boolalpha << UserType::_FullName() << "= FBMetadata({ Name = '" << LuaName() << "', "
                << UserType::GetMetaParams() << "Hash = 1001, Vars = { \n";
            int field_id = 0;
            for (ScadaFields::LuaMeta::const_iterator it = meta.begin(); it != meta.end(); ++it) {
                const char* type = it->second->lua_type();
                if (type == nullptr)
                    continue;
                oss << "	" << it->first << " = { "
                    << "Hash = " << field_id++ << ',' << "Type = " << type << ',' << "Retain = " << it->second->retain
                    << " },\n";
            }
            oss << "}})\n";
            return oss.str();
        }
        static int Length(lua_State* L) {
            lua_pushinteger(L, fields.size());
            return 1;
        }
        static void to_json(mplc::JsonWrapper& json, const UserType& obj) {
            json.value.SetObject();
            for (ScadaFields::LuaMeta::const_iterator it = fields.meta.begin(); it != fields.meta.end(); ++it) {
                Value js_key, js_val;
                js_key.SetString(it->first.data(), it->first.size(), json.allocator);
                mplc::JsonWrapper tmp(js_val, json.allocator);
                it->second->to_json(tmp, &obj);
                json.value.AddMember(js_key, js_val, json.allocator);
            }
        }
        static void BindFields() {
            detail::BindFields<UserType, UserType>::bind();
            HasDynamicFields<UserType>::bind();
        }
        static int RegFBType(lua_State* L) {
            const char* lib_name = LuaName();
            static const luaL_Reg Lib_f[] = {{"new", New},
                                             {"get", Get},
                                             {"set", Set},
                                             {"call", Call},
                                             {nullptr, nullptr}};

            static const luaL_Reg Lib_m[] = {{"__index", Get},
                                             {"__newindex", Set},
                                             {"__call", Call},
                                             {"__gc", GC},
                                             {"__len", Length},
                                             {nullptr, nullptr}};
            BindFields();
            RegCustomType(L, Lib_m, Lib_f);
            return 0;
        }
        static int RegStructType(lua_State* L) {
            static const luaL_Reg Lib_f[] = {{"new", New},
                                             {"get", Get},
                                             {"set", Set},
                                             {"clone", Clone},
                                             {nullptr, nullptr}};

            static const luaL_Reg Lib_m[] = {{"__index", Get}, {"__newindex", Set}, {"__gc", GC}, {nullptr, nullptr}};
            BindFields();
            RegCustomType(L, Lib_m, Lib_f);
            return 0;
        }

        static int RegCustomType(lua_State* L, const luaL_Reg* Lib_m, const luaL_Reg* Lib_f) {
            const char* lib_name = LuaName();

            /*if (Lib_f) {
                lua_createtable(L, 0, 0);
                luaL_setfuncs(L, Lib_f, 0);
            }
            if (Lib_m) {
                luaL_newmetatable(L, lib_name);
                luaL_setfuncs(L, Lib_m, 0);
                if (Lib_f) {
                    lua_setmetatable(L, -2);
                }
            }
            if (Lib_m || Lib_f)
                lua_setglobal(L, lib_name);*/
            luaL_newmetatable(L, lib_name);
            luaL_setfuncs(L, Lib_m, 0);
            lua_pop(L, 1);
            if (Lib_f) {
                lua_createtable(L, 0, 0);
                luaL_setfuncs(L, Lib_f, 0);
                lua_setglobal(L, lib_name);
            }
            return 0;
        }
    };
    template<class UserType>
    ScadaFields ScadaObj<UserType>::fields;  //    = new ScadaFields();
    template<class T>
    static int RegisterFbType() {
        int res = RegisterExternalSTLib(T::_ShortName(), ScadaObj<T>::RegFBType);
        if (res != S_OK) {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR,
                        BOOST_STRINGIZE(Class) ": Already registred other type with name '%s'",
                        T::_ShortName());
        }
        return res;
    }
    template<class T>
    static int RegisterStructType() {
        int res = RegisterExternalSTStruct(T::_ShortName(), ScadaObj<T>::RegStructType);
        if (res != S_OK) {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR,
                        BOOST_STRINGIZE(Class) ": Already registred other type with name '%s'",
                        T::_ShortName());
        }
        return res;
    }
    // struct FB_TYPE {
    //    static int __index(lua_State* L) { return 0; }
    //    static int __newindex(lua_State* L) { return 0; }
    //    static int __call(lua_State* L) { return 0; }
    //    static int __gc(lua_State* L) { return 0; }
    //    static int __len(lua_State* L) { return 0; }
    //};

    // void test(lua_State* L) {
    //    static const luaL_Reg Lib_m[] = {{"__index", FB_TYPE::__index},
    //                                     {"__newindex", FB_TYPE::__newindex},
    //                                     {"__call", FB_TYPE::__call},
    //                                     {"__gc", FB_TYPE::__gc},
    //                                     {"__len", FB_TYPE::__len},
    //                                     {nullptr, nullptr}};

    //}
};  // namespace SCADA_API
