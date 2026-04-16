#ifndef STStandartFB_h
#define STStandartFB_h
#include "mplc_stdint.h"
#undef IN
#undef OUT
BEGIN_EXTERN_C
//---------ST-FB------------//
LUA_API int(luaopen_CTD)(lua_State* L);
LUA_API int(luaopen_CTD_DINT)(lua_State* L);
LUA_API int(luaopen_CTD_LINT)(lua_State* L);
LUA_API int(luaopen_CTD_UDINT)(lua_State* L);
LUA_API int(luaopen_CTU)(lua_State* L);
LUA_API int(luaopen_CTU_DINT)(lua_State* L);
LUA_API int(luaopen_CTU_LINT)(lua_State* L);
LUA_API int(luaopen_CTU_UDINT)(lua_State* L);
LUA_API int(luaopen_CTU_ULINT)(lua_State* L);
LUA_API int(luaopen_CTUD)(lua_State* L);
LUA_API int(luaopen_CTUD_DINT)(lua_State* L);
LUA_API int(luaopen_CTUD_LINT)(lua_State* L);
LUA_API int(luaopen_CTUD_ULINT)(lua_State* L);
LUA_API int(luaopen_RS)(lua_State* L);
LUA_API int(luaopen_SR)(lua_State* L);
LUA_API int(luaopen_TOF)(lua_State* L);
LUA_API int(luaopen_TON)(lua_State* L);
LUA_API int(luaopen_TP)(lua_State* L);
LUA_API int(luaopen_R_TRIG)(lua_State* L);
LUA_API int(luaopen_F_TRIG)(lua_State* L);
LUA_API int(luaopen_AlarmCondition)(lua_State* L);
LUA_API int(luaopen_SetDateAndTime)(lua_State* L);

LUALIB_API int(luaopen_StandartFunctions)(lua_State* L);

extern int luaopen_Retain(lua_State* L);
#ifndef ST_SCADA
extern int luaopen_MUTEX(lua_State* L);
#endif
extern int luaopen_CSTD_LIB(lua_State* L);

END_EXTERN_C

#define ST_CTD_LIBNAME "CTD"
#define ST_CTD_DINT_LIBNAME "CTD_DINT"
#define ST_CTD_LINT_LIBNAME "CTD_LINT"
#define ST_CTD_UDINT_LIBNAME "CTD_UDINT"
#define ST_CTU_LIBNAME "CTU"
#define ST_CTU_DINT_LIBNAME "CTU_DINT"
#define ST_CTU_LINT_LIBNAME "CTU_LINT"
#define ST_CTU_UDINT_LIBNAME "CTU_UDINT"
#define ST_CTU_ULINT_LIBNAME "CTU_ULINT"
#define ST_CTUD_LIBNAME "CTUD"
#define ST_CTUD_DINT_LIBNAME "CTUD_DINT"
#define ST_CTUD_LINT_LIBNAME "CTUD_LINT"
#define ST_CTUD_ULINT_LIBNAME "CTUD_ULINT"
#define ST_RS_LIBNAME "RS"
#define ST_R_TRIG_LIBNAME "R_TRIG"
#define ST_F_TRIG_LIBNAME "F_TRIG"
#define ST_SR_LIBNAME "SR"
#define ST_TOF_LIBNAME "TOF"
#define ST_TON_LIBNAME "TON"
#define ST_TP_LIBNAME "TP"
#define ST_SetDateAndTime_LIBNAME "SetDateAndTime"
#define ST_StandartFunctions_LIBNAME "StandardFunctionsC"
#define ST_RegisteredFunctions_LIBNAME "RegisteredFunctions"

////////////////////////////////////

typedef struct R_TRIG {
    FBData Data;
    BOOLEAN CLK, Q, M;
} R_TRIG;

typedef struct F_TRIG {
    FBData Data;
    BOOLEAN CLK, Q, M, empty;
} F_TRIG;

extern void Invoke_R_TRIG(struct R_TRIG* trig);

extern void Invoke_F_TRIG(struct F_TRIG* trig);

extern BOOLEAN Set_R_EDGE(struct R_TRIG* trig, BOOLEAN clk);

extern BOOLEAN Set_F_EDGE(struct F_TRIG* trig, BOOLEAN clk);

////////////////////////////////////
typedef struct SR {
    FBData Data;
    BOOLEAN S1, R, Q1, empty;
} SR;

extern void(Invoke_SR)(SR* sr);

typedef struct RS {
    FBData Data;
    BOOLEAN S, R1, Q1, empty;
} RS;

extern void Invoke_RS(RS* sr);

/////////////////////////////

typedef struct CTU {
    FBData Data;
    LeadEdge CU;
    SHORT PV, CV;
    BOOLEAN R, Q;
} CTU;

extern void Invoke_CTU(CTU* ctu);

typedef struct CTU_DINT {
    FBData Data;
    LeadEdge CU;
    INT PV, CV;
    BOOLEAN R, Q;
} CTU_DINT;

extern void Invoke_CTU_DINT(CTU_DINT* ctu);

typedef struct CTU_LINT {
    FBData Data;
    LeadEdge CU;
    int64_t PV, CV;
    BOOLEAN R, Q;
} CTU_LINT;

extern void Invoke_CTU_LINT(CTU_LINT* ctu);

typedef struct CTU_UDINT {
    FBData Data;
    LeadEdge CU;
    UINT PV, CV;
    BOOLEAN R, Q;
} CTU_UDINT;

extern void Invoke_CTU_UDINT(CTU_UDINT* ctu);

typedef struct CTU_ULINT {
    FBData Data;
    LeadEdge CU;
    uint64_t PV, CV;
    BOOLEAN R, Q;
} CTU_ULINT;

extern void Invoke_CTU_ULINT(CTU_ULINT* ctu);

////////////////////////////////////////////////////////

typedef struct CTD {
    FBData Data;
    LeadEdge CD;
    SHORT PV, CV;
    BOOLEAN Q, LD;
} CTD;

extern void Invoke_CTD(CTD* ctd);

typedef struct CTD_DINT {
    FBData Data;
    LeadEdge CD;
    INT PV, CV;
    BOOLEAN Q, LD;
} CTD_DINT;

extern void Invoke_CTD_DINT(CTD_DINT* ctd);

typedef struct CTD_LINT {
    FBData Data;
    LeadEdge CD;
    LONG PV, CV;
    BOOLEAN Q, LD;
} CTD_LINT;

extern void Invoke_CTD_LINT(CTD_LINT* ctd);

typedef struct CTD_UDINT {
    FBData Data;
    LeadEdge CD;
    UINT PV, CV;
    BOOLEAN Q, LD;
} CTD_UDINT;

extern void Invoke_CTD_UDINT(CTD_UDINT* ctd);

//////////////////////////////////////////////////////

typedef struct CTUD {
    FBData Data;
    LeadEdge CU, CD;
    SHORT PV, CV;
    BOOLEAN R, LD, QU, QD;
} CTUD;

extern void Invoke_CTUD(CTUD* ctud);

typedef struct CTUD_DINT {
    FBData Data;
    LeadEdge CU, CD;
    int32_t PV, CV;
    BOOLEAN R, LD, QU, QD;
} CTUD_DINT;

extern void Invoke_CTUD_DINT(CTUD_DINT* ctud);

typedef struct CTUD_LINT {
    FBData Data;
    LeadEdge CU, CD;
    int64_t PV, CV;
    BOOLEAN R, LD, QU, QD;
} CTUD_LINT;

extern void Invoke_CTUD_LINT(CTUD_LINT* ctud);

typedef struct CTUD_ULINT {
    FBData Data;
    LeadEdge CU, CD;
    uint64_t PV, CV;
    BOOLEAN R, LD, QU, QD;
} CTUD_ULINT;

extern void Invoke_CTUD_ULINT(CTUD_ULINT* ctud);

//////////////////////////////////////////////////////

typedef struct TP {
    FBData Data;
    double PT, ET;
    double safePT;
    DWORD startTime;
    int state;
    BOOLEAN IN;
    BOOLEAN Q;
} TP;

extern void Invoke_TP(TP* tp);

typedef struct TON {
    FBData Data;
    double PT, ET;
    double safePT;
    DWORD startTime;
    int state;
    BOOLEAN IN, Q;
} TON;

extern void Invoke_TON(TON* ton);

typedef struct TOF {
    FBData Data;
    double PT, ET;
    double safePT;
    DWORD startTime;
    int state;
    BOOLEAN IN, Q;
} TOF;

extern void Invoke_TOF(TOF* tof);

typedef struct SetDateAndTime {
    FBData Data;
    FILETIME DateAndTime;
    BOOLEAN IsUTC;
    BOOLEAN Execute;
    BOOLEAN OldExecute;
    int ErrorCode;
} SetDateAndTime;

extern void Invoke_SetDateAndTime(SetDateAndTime* SetDateAndTime);

#endif
