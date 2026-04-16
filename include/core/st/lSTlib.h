#ifndef ST_LIB_h
#define ST_LIB_h
//

#ifndef LUA_EXTERNAL_PLC
#    include "fbexec.h"
#endif
//#include "hash.h"

BEGIN_EXTERN_C

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

typedef struct FBData {
    int VarValuesRef;
    //	hash* BlockedParams;
    BOOLEAN EN, EnO; /* VVB-120206(WI4524) */
} FBData;

static void InitFBData(FBData PTR data) {
    data->VarValuesRef = LUA_NOREF;
    //	data->BlockedParams = NULL;
    data->EN = data->EnO = 1;
}

extern int GetIntField(lua_State* L, const char* key);
extern long GetLongField(lua_State* L, const char* key);
extern double GetDoubleField(lua_State* L, const char* key);
extern FILETIME GetFTField(lua_State* L, const char* key);
extern BOOLEAN GetBoolField(lua_State* L, const char* key);
extern void GetStringField(ST_STRING* s, lua_State* L, const char* key);
extern int CheckIndexKey(lua_State* L, const char* key, FBData PTR data);
extern int PushInvalidKeyMessage2(lua_State* L, const char* key, FBData PTR data);
extern int PushInvalidKeyMessage(lua_State* L, const char* key);
extern void InitEnEnO(lua_State* L, FBData PTR data);
extern int EnEnOIndex(lua_State* L, const char* key, FBData PTR data);
extern int EnEnONewIndex(lua_State* L, const char* key, FBData PTR data);
extern int CheckIndexProlog(lua_State* L, const char* key, FBData PTR data);
extern int CheckNewIndexEpilog(lua_State* L, const char* key, FBData PTR data);
extern int CheckNewIndexProlog(lua_State* L, const char* key, FBData PTR data);
extern BOOLEAN GetStringValue(ST_STRING* s, lua_State* L, int n);
// extern BOOLEAN FBData_SetBlocked(lua_State *L, FBData PTR data);
// extern BOOLEAN FBData_IsBlocked(const char* key, FBData PTR data);

extern void stackDump(lua_State* L);
LUALIB_API void luaL_openSTlibs(lua_State* L);
LUALIB_API void luaL_openSTExternalslibs(lua_State* L, const char* fbList, int fbListSize);

#ifndef LUA_EXTERNAL_PLC
extern int RegisterExternalSTLib(const char* name, lua_CFunction func);
extern int RegisterExternalSTFunc(const char* name, lua_CFunction func);
extern int RegisterExternalSTStruct(const char* name, lua_CFunction func);

extern int GetSTTaskLuaState(int taskId, int stateType, lua_State** pL);
extern int GetSTTasksList(int* tasksCount, int** taskIdList);
extern int FindSTTask(lua_State* L);
extern int LuaSetNeedAutolock(int taskId, RBOOLEAN autoLock);
extern int LuaSetLock(int taskId, RBOOLEAN bLock);

extern unsigned int GetUAQualityByDA(short quality);
extern short GetDAQualityByUA(unsigned int input);

extern int LuaSetErrorHandler(lua_State* L);

typedef int (*RTErrorHandlerFunc)(lua_State* L, const char* msg);
extern int CallRTErrorHandler(lua_State* L, const char* msg);
extern int SetRTErrorHandler(RTErrorHandlerFunc func);

typedef int (*GetLuaValueAsBufferFunc)(lua_State* L, DYN_MEM* mem);
typedef int (*SetLuaValueFromBufferFunc)(lua_State* L, DYN_MEM* mem, int size);
extern int CallRTErrorHandler(lua_State* L, const char* msg);
extern int SetLuaValueBufferFuncs(GetLuaValueAsBufferFunc funcGet, SetLuaValueFromBufferFunc funcSet);

extern void read_lua_value_to_RPARAM(RPARAM* param, lua_State* L);

#endif

END_EXTERN_C

#endif
