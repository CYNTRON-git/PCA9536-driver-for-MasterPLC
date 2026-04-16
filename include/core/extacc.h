#pragma once
#include "main.h"
//#include "mt_core.h"
#include "task.h"

enum extAccessTaskFlags
{
	eafUseExtendedConfig = 0x1
};

//enum ExtAccessParamFlags
//{
//	eapfEnableWrite = 0x1,
//	eapfHistorizing = 0x2,
//};

enum RWAccessTypes { rwatRead = 0, rwatWrite = 1, rwatReadWrite = 2 };

_PACKED(typedef struct EXTACCESS_TASK_CONFIG
{
	TASK_HEADER Header;
	DWORD		dwFlags;
    DWORD		dwMaxExtAccessSize;
    DWORD		dwQuanParams;
	WORD		wParamSize;
}) EXTACCESS_TASK_CONFIG;

_PACKED(typedef struct EXTACCESS_ITEM
{
    WORD  AccessType;
    int64_t ChannelId;  // Идентификатор параметра, привязанного к внешнему каналу
    int64_t ReadItemId;
    int64_t WriteItemId;
	WORD  ModbusAddress;
	DWORD ParamFlags; //ExtAccessParamFlags
	BYTE  ModbusType; //EModbusValueType
	WORD  Off;
	WORD  Len;
	int  IEC104Address;
})  EXTACCESS_ITEM;

typedef struct EXTACCESS_TASK_CB
{
	EXTACCESS_TASK_CONFIG 	PTR pHeader;
	EXTACCESS_ITEM			PTR pListItem;
	TASK_PARAM				PTR	pListParam;
	BYTE					PTR OffsetId;				
} EXTACCESS_TASK_CB;

extern EXTACCESS_TASK_CB ExtAccessTaskCB;
extern int LoadExtAccessTaskConfig(EXTACCESS_TASK_CONFIG PTR pHeader);
extern int InitExtAccessTask(int task);
extern int ReadExtAccess(WORD fn,WORD mba,WORD Count,BYTE* pbData);
extern int WriteExtAccess(WORD fn,WORD mba,WORD Count,BYTE* pbData);
extern int ExtAccessTaskFinish(int task);

EXTERN_C EXTACCESS_TASK_CB PTR GetExtAccessTaskCB();
EXTERN_C int GetExtAccessTaskParamName(int param, char* buf, int bufLen);

