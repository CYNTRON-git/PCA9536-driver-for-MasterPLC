#ifndef LUA_ADDINS_H
#define LUA_ADDINS_H

#include <lua.hpp>

#include "st/lSTlib.h"
#include "st/STTime.h"

#define SYSTEM_PARAM_QUALITY_FIELD "StatusCode"
#define SYSTEM_PARAM_VALUE_FIELD "Value"
#define SYSTEM_PARAM_TIME_FIELD "SourceTime"

/* clang-format off */
//MPLCSHARE_API OpcUa_StatusCode ReadJSON(OpcUa_VariantHlp& pValue, lua_State* L);
MPLCSHARE_API OpcUa_StatusCode ReadLuaValue(OpcUa_VariantHlp& v, lua_State *L, int idx, const OpcUa_BuiltInTypeHlp& typeHlp);
MPLCSHARE_API void ReadLuaValueOpcUaVariantTableRaw(OpcUa_Variant& v, lua_State *L, int idx, const OpcUa_BuiltInTypeHlp& typeHlp);
MPLCSHARE_API bool GetLuaStringValue(std::string& s, lua_State *L, int n);  // TODO удалить мусор
MPLCSHARE_API bool GetLuaTimeValue(OpcUa_DateTime& dt, lua_State *L, int n);
MPLCSHARE_API bool GetLuaTimeValue(int64_t& dt, lua_State* L, int n); 
//MPLCSHARE_API OpcUa_StatusCode WriteLuaValue(const OpcUa_Variant& v, lua_State* L, const OpcUa_BuiltInTypeHlp* typeHlp = nullptr);
//MPLCSHARE_API void WriteLuaValueOpcUaVariantTableRaw(const OpcUa_Variant& v, lua_State *L, OpcUa_BuiltInType valueBuiltInType);
MPLCSHARE_API void WriteLuaTimeValue(const OpcUa_DateTime& dt, lua_State *L);
MPLCSHARE_API void WriteLuaTimeValue(int64_t dt, lua_State* L);
MPLCSHARE_API void* ReadLuaFB(lua_State *L, int idx, const std::string& name);
/* clang-format on */

inline bool PushLuaStringValue(lua_State* L, const std::string& s) {
    std::string s1251;
    if(OpcUa_IsGood(OpcUa_VariantHlp::ConvertFromUTF8String(s1251, s))) {
        lua_pushlstring(L, s1251.c_str(), s1251.size());
        return true;
    } else {
        lua_pushnil(L);
        return false;
    }
}

FILETIME mplc_checkFT(lua_State* L, int n);
MPLCSHARE_API int mplc_newFTfromFT(lua_State* L, FILETIME d);
// Выводит значения стека
// MPLCSHARE_API void TraceLuaStackD(lua_State * L, int Begin, int End);

MPLCSHARE_API void RegisterLuaValueBufferFuncs();

#endif
