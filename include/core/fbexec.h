#ifndef FBEXEC_H
#define FBEXEC_H

#include "main.h"
#include "task.h"
#include "mt_core.h"
#include "fb.h"



//typedef enum USER_TASK_FLAGS
//{ 
//	utfCommandStop = 0x1,
//	utfStopped = 0x2,
//	utfAllowOneStep = 0x4
//} R_PACKED USER_TASK_FLAGS;

//#define	utfCommandStop 0x1
//	utfStopped = 0x2,
//	utfAllowOneStep = 0x4

typedef unsigned char USER_TASK_FLAGS;
static const USER_TASK_FLAGS utfCommandStop = 0x1;
static const USER_TASK_FLAGS utfStopped = 0x2;
static const USER_TASK_FLAGS utfAllowOneStep = 0x4;
static const USER_TASK_FLAGS utfStatsControlledByFB = 0x8;
static const USER_TASK_FLAGS utfIsDriverTask = 0x10;
static const USER_TASK_FLAGS utfDisableAutoLock = 0x20;

struct LUA_TASK_CB;

_PACKED(typedef struct TASK_STATS
{
	DWORD  LenLastStep; // Полседная длительность времени выполнения 
	int  MinTime;
	int  MaxTime;
	int64_t  cicle_counter; // Кол-во циклов выполнения
	DWORD  LastTick;
	double AllTime;
}) /*R_PACKED*/ TASK_STATS;

#define USER_TASK_STAT_COUNT	3

typedef struct FBEXEC_CB
{
	TASK_HEADER_EX PTR pHeader;
	TASK_PARAM  PTR pListParams;
	RPARAM PTR  pParams;

	FB_CB PTR   pListFB;
	BYTE PTR    pShareMemFB;
	DYN_MEM FBMem;
	DYN_MEM LoadMem;  // pointer to FB personal memory, loaded from hotrestart

	
	DYN_MEM LoadMemBlocked; // read on slave | 
	DYN_MEM MemBlocked; // write all     | 

	WORD              QuanFB;
	//WORD              SizeShareMemFB;
	int LoadMemVersion;
	RBOOLEAN UsePersonalMem;
	struct LUA_TASK_CB PTR pLuaTaskCB;

	DWORD             UT_time_msec;
	RTIME           UT_date_time;
	WORD            TaskFlags;

	TASK_STATS Stat;
	TASK_STATS WorkStat;
	DWORD      Stats[USER_TASK_STAT_COUNT];
    DWORD error_counter;

	WORD			TaskIndex;
	short			LuaTaskIndex;
	short           StopInFB;
	WORD            NextExecutingFB;
	WORD            ReserveArrayVersion;
	WORD            ReserveArrayWriteVersion;

	int64_t luaHeapSize;
} /*R_PACKED */ FBEXEC_CB;

struct lua_State;

typedef int (*STProcessorCallback)(void* data, STProcessorMode mode, int taskId);
EXTERN_C int RegisterSTProcessor(int taskId,
	LPCSTR name,
	uint32_t modeMask,
	STProcessorCallback func,
	void* data,
	int priority,
	int* pnIndex);
EXTERN_C int UnRegisterSTProcessor(int taskId, int nIndex, void* data);

#define MAX_ST_PROCESSORS	10
typedef struct STProcessorDef
{
	DWORD modeMask;
	int priority;
	LPCSTR name;
	STProcessorCallback func;
	void* data;
} STProcessorDef;

typedef struct LUA_TASK_CB {
	int LuaTaskIndex;
	struct lua_State* LuaManagerThread;
	struct lua_State* LuaVarSurvayThread;
	FBEXEC_CB* pFBExecCB;
#ifndef LUADLL
	R_CRITICAL_SECTION luaCriticalSection; //TODO Сделать блокировку на уровне отдельных задач вместо общего csSubscribeControl
#endif
	R_CRITICAL_SECTION csSubscribeControl;
	WORD ReserveArrayWriteVersion;
	RBOOLEAN GCOnEveryCycle;

	int STProcessorCount;
	STProcessorDef STProcessorDefs[MAX_ST_PROCESSORS];
} LUA_TASK_CB;

extern LUA_TASK_CB LuaTasksCB[];

extern int QuanLuaTasks;

extern int PreInitFBExec(FBEXEC_CB PTR pFBExecCB);
extern int InitFBExec(FBEXEC_CB PTR pFBExecCB);
extern void DisposeFBExec(FBEXEC_CB PTR pFBExecCB);
extern int FBExecOneStep(FBEXEC_CB PTR pFBExecCB, BYTE fbMask);
//extern int FBExecSetParams(FBEXEC_CB PTR pFBExecCB);
//extern int FBExecGetOutParams(FBEXEC_CB PTR pFBExecCB);
EXTERN_C FBEXEC_CB PTR GetFBParentTask(FB_CB PTR fbCB);
EXTERN_C FBEXEC_CB PTR GetTaskSettings(int taskId);
extern int TaskCopyFromReserv(FBEXEC_CB PTR pFBExecCB);

EXTERN_C void InitTaskStats(TASK_STATS* stats);
EXTERN_C void TaskStatsTick(TASK_STATS* stats);
EXTERN_C void TaskStatsStart(TASK_STATS* stats);
EXTERN_C void PrintTaskStats(TASK_STATS* stats);

#endif // #ifndef FBEXEC_H
