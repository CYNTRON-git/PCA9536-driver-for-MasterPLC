#ifndef USER_TASK_H
#define USER_TASK_H

#include "fbexec.h"

_PACKED(typedef struct USER_TASK_CB
    {
    TASK_HEADER_EX PTR pHeader;
	FBEXEC_CB PTR pFBExecCB;

    RHANDLE           RPID;
    DWORD             UT_start_cycle;
 })  USER_TASK_CB;

extern USER_TASK_CB PTR UserTasksCB;

extern USER_TASK_CB PTR GetUserTaskCB(int index);

typedef int (*ReadGlobalParamFunc)(int64_t id, RPARAM PTR value);
typedef int (*WriteGlobalParamFunc)(int64_t id, RPARAM PTR value, RBOOLEAN bBlockOne);
EXTERN_C void SetGlobalParamFuncs(ReadGlobalParamFunc readFunc, WriteGlobalParamFunc writeFunc);
int LoadFbMem(FBEXEC_CB PTR pFBExecCB, BYTE* buf, DWORD size, BYTE bVersion);


#if defined(IPC_ANY) || defined(PC_MSDOS) || defined(PC_WIN32) || defined(FALCOM) || defined(PC_ADAM) || defined(LINUX) || defined(ECOS)
  #if defined(MTCORE_PROC)
    extern void UserTaskThread(PID *pid, WORD Task);
  #elif defined(PC_WIN32)
    extern DWORD UserTaskThread(LPVOID lpThreadParameter);
  #elif defined(LINUX) || defined(ECOS)
      extern void * UserTaskThread(void *lpThreadParameter);
  #else
    #error MT core not defined !!!
  #endif
#endif

#endif // #ifndef USER_TASK_H
