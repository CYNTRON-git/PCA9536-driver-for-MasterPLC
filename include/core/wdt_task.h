#ifndef WDT_TASK_H
#define WDT_TASK_H


#include "main.h"
#include "mt_core.h"

#include "wdt_funcs.h"


#ifndef COOP_MTASK

extern RHANDLE WDT_Task_RPID;

extern volatile DWORD WDT_Task_Mask;
extern volatile DWORD WDT_Task_Running;
extern volatile DWORD g_dwWDTStartTimeout;


void WDT_Task_Finish(void);
#if defined(MTCORE_PROC)
    #define __WDT_Task_Thread_DECL void WDT_Task_Thread(PID *pid, WORD Task)
#elif defined(_WIN32)
    #define __WDT_Task_Thread_DECL DWORD WDT_Task_Thread(LPVOID lpThreadParameter)
#elif defined(LINUX) || defined(ECOS)
    #define __WDT_Task_Thread_DECL DWORD WDT_Task_Thread(void *lpThreadParameter)
#else
    #error MT core not defined !!!
#endif
__WDT_Task_Thread_DECL;

#endif

#define WDT_GROUP_USER_TASK	0
#define WDT_GROUP_SERIAL_TASK	1
#define WDT_GROUP_OTHER_CONTR_TASK	2

enum WDMode {
    wdmStart = 0,
    wdmRefresh = 1,
    wdmStop = 2
};
typedef int (*WatchdogFunc)(int mode, int param);

EXTERN_C int SetWatchdogFunc(WatchdogFunc func);
void CallWatchdogFunc(int mode, int param);

#endif
