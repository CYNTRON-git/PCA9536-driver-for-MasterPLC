#ifndef MT_CORE_H
#define MT_CORE_H

#ifdef MTCORE_PROC
  #include "proc/proc.h"
#endif

extern void RMTCoreInit(void);
extern void RMTCoreInitMem(int nTaskCount);

extern RHANDLE RMTCreateThread(
    //void (*pThreadProc)(),
    void*           pThreadProc,
    int             Priority,
    DWORD           StackSize,
    void PTR  pThreadParam );

extern RHANDLE RMTCreateThreadLocalStack(
    void*           pThreadProc,
    int             Priority,
	BYTE*			pStack,
    DWORD           StackSize,
    void PTR  pThreadParam );

extern void RMTTerminateThread(RHANDLE hThread);

extern void RMTCoreStart(void);

extern void RSleepLock_ms(int sl_ms);
extern RBOOLEAN IsSystemTimeChanged(DWORD StartTime, DWORD CurTime);

#endif // #ifndef MT_CORE_H
