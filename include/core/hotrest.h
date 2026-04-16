#pragma once

#include "main.h"
#include "main_imp.h"
#include "mt_core.h"
#include "task.h"

_PACKED(typedef struct HOTRESTART_TASK_CONFIG
{
	TASK_HEADER Header;
	WORD		wFlags;
	WORD		wSavePeriod;
	DWORD		dwActualPeriod;
	char		szLocation_old[20];
	WORD		QuanBooleanParams;
	WORD		QuanIntegerParams;
	WORD		QuanFloatParams;
})  HOTRESTART_TASK_CONFIG;

enum SaveStateModes { ssmPeriodic, ssmOnStop, ssmNone };

_PACKED(typedef struct HOTRESTART_TASK_CB
{
	UNALIGNED HOTRESTART_TASK_CONFIG PTR pHeader;
	DYN_MEM tempArray;
	DYN_MEM currentArray[2];
	BYTE PTR pTempBuf;
	char* szLocation[2];
	char* szImportLocation;
	char* szBackupLocation;
	int curIndex;
    enum SaveStateModes SaveStateMode;

	DWORD cicle_counter;
	DWORD LenLastStep_msec;
    DWORD StartLastStep_msec;
	DWORD error_counter;
	DWORD LastArrayRead[2];

	WORD ReserveArrayChangesCounter;
	RBOOLEAN8 bOldSaveCommand;
	RBOOLEAN8 bCurrentArrayRead[2];
	RBOOLEAN8 bDisableSave;
	RBOOLEAN8 IsLoadedSameVersion;
}) HOTRESTART_TASK_CB;

_PACKED(typedef struct HOTRESTART_CONTENT_HEADER
{
	RTIME	RTime;
	BYTE	bVersion;
    BYTE	bReserv;
	DWORD	dwSizeOfReservArray;
	BYTE	pbConfigId[16];
	WORD	wCrc16;
	WORD	wReserv1;
})  HOTRESTART_CONTENT_HEADER;

extern int InitHotrestartTask(void);
//extern int DoHotrestartTaskStep(void);
extern int HotrestartTaskFinish(void);
extern int DeleteHotrestartState(void);

extern HOTRESTART_TASK_CB HotrestartTaskCB;
#ifdef USE_STATIC_STACK
extern char HotrestartTask_ws[TASK_WS_SIZE];
#endif

DECLARE_THREAD(HotrestartTaskThread);
 
extern RHANDLE HotrestartTaskRPID;
extern RBOOLEAN8 ExportHotRestart;
extern RBOOLEAN8 ImportHotRestart;
extern RBOOLEAN8 NeedDeleteHotRestart;

typedef int (*Init_BackupRAMFunc)(int size);
typedef int (*MemCpy_From_BackupRAMFunc)(void* dst, DWORD addr, DWORD size);
typedef int (*MemCpy_To_BackupRAMFunc)(void* hdr, DWORD hdr_size, void* src, DWORD size);

EXTERN_C int SetBackupRAMFuncs(Init_BackupRAMFunc init_func,
                               MemCpy_From_BackupRAMFunc read_func,
                               MemCpy_To_BackupRAMFunc write_func);


