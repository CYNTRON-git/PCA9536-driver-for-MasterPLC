#ifndef __MACROS_H__
#define __MACROS_H__

///////////////////////////////////

//#define to_str(...) #__VA_ARGS__
//#define LIB_NAME(FBName) to_str(FBName)

//#define to_str(...) #__VA_ARGS__
#define LIB_NAME(FBName) #FBName

#define CreateFBUtil(FBName, Lua_FB_Name)                                                                              \
    LuaFBUtil FBUtil_##FBName(Lua_FB_Name);                                                                            \
                                                                                                                       \
    static int NewFB_##FBName(lua_State* L) {                                                                          \
        FBName* fb = new (lua_newuserdata(L, sizeof(FBName))) FBName(L, Lua_FB_Name);                                  \
        fb->Init(L);                                                                                                   \
        return 1;                                                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    static int Call_##FBName(lua_State* L) { return ((FBName*)FBUtil_##FBName.check(L))->call(L); }                    \
                                                                                                                       \
    static int Get_##FBName(lua_State* L) { return FBUtil_##FBName.check(L)->getField(L); }                            \
    template<class T>                                                                                                  \
    static int GC(lua_State* L) {                                                                                      \
        ((T*)FBUtil_##FBName.check(L))->~T();                                                                          \
        return 1;                                                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    static int Set_##FBName(lua_State* L) { return FBUtil_##FBName.check(L)->setField(L); }

//////////////////////////////////
#define RegLuaFB(FBName, Lua_FB_Name)                                                                                  \
    static const luaL_Reg FBName##_Lib_f[] = {{"new", NewFB_##FBName},                                                 \
                                              {"get", Get_##FBName},                                                   \
                                              {"set", Set_##FBName},                                                   \
                                              {"call", Call_##FBName},                                                 \
                                              {NULL, NULL}};                                                           \
    static const luaL_Reg FBName##_Lib_m[] = {{"__index", Get_##FBName},                                               \
                                              {"__newindex", Set_##FBName},                                            \
                                              {"__call", Call_##FBName},                                               \
                                              {"__gc", GC<FBName>},                                                    \
                                              {NULL, NULL}};                                                           \
    LUA_API int luaopen_##FBName(lua_State* L) {                                                                       \
        lua_createtable(L, 0, 0);                                                                                      \
        luaL_setfuncs(L, FBName##_Lib_f, 0);                                                                           \
                                                                                                                       \
        luaL_newmetatable(L, Lua_FB_Name);                                                                             \
        luaL_setfuncs(L, FBName##_Lib_m, 0);                                                                           \
        lua_setmetatable(L, -2);                                                                                       \
        lua_setglobal(L, Lua_FB_Name);                                                                                 \
        const std::string& metadata = BaseFB<FBName>::MakeMetadata(Lua_FB_Name);                                       \
        int error = luaL_loadbuffer(L, metadata.c_str(), metadata.size(), Lua_FB_Name) ||                              \
                    lua_pcall(L, 0, LUA_MULTRET, 0);                                                                   \
        if (error) {                                                                                                   \
            PRINTLN("%s", lua_tostring(L, -1));                                                                       \
            lua_pop(L, 1); /* pop error message from the stack */                                                      \
            return E_FAIL;                                                                                             \
        }                                                                                                              \
        return 1;                                                                                                      \
    }                                                                                                                  \
    extern int RegisterSTLibs##FBName() {                                                                              \
        RegisterExternalSTLib(Lua_FB_Name, luaopen_##FBName);                                                          \
        return S_OK;                                                                                                   \
    }

/////////////////////////////////
#define REGISTER_FB_IMPL(FBName) CreateFBUtil(FBName, LIB_NAME(FBName)) RegLuaFB(FBName, LIB_NAME(FBName))

#define REGISTER_FB_IMPLWITH_NAME(FBName, USER_NAME) CreateFBUtil(FBName, USER_NAME) RegLuaFB(FBName, USER_NAME)
#define REGISTER_FB_CALL(FBName) RegisterSTLibs##FBName();

#define REGISTER_FB_DECLARE(FBName) extern int RegisterSTLibs##FBName();

#define BEGIN_PARAM_MAP()                                                                                              \
public:                                                                                                                \
    const static fb_input_meta* GetMetaTable() {                                                                       \
        static const fb_input_meta _entries[] = {
#define IN_PARAM(Name, Type) {Name, ParamType::__##Type},

#define OUT_PARAM(Name, Type, Value) {Name, ParamType::__##Type},

#define END_PARAM_MAP()                                                                                                \
    { nullptr, 0 }                                                                                                     \
    }                                                                                                                  \
    ;                                                                                                                  \
    return &_entries[0];                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
private:

#endif  //__MACROS_H__
