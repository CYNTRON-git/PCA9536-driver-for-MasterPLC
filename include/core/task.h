#ifndef TASK_H
#define TASK_H

#include "main.h"



_PACKED (typedef struct TASK_HEADER
    {
    DWORD  CfgSize;
    WORD   Number;
    BYTE   Priority;
    DWORD  Period;
    BYTE   Type;
    })  TASK_HEADER;

_PACKED(typedef struct TASK_HEADER_EX
{
    DWORD  CfgSize;
    WORD   Number;
    BYTE   Priority;
    DWORD  Period;
    BYTE   Type;
    DWORD   QuanParams;
} )
TASK_HEADER_EX;



enum EParamFlags
{
	pfOutputParam = 0x1,
	pfDisableArchivingByChange = 0x2
};

_PACKED(typedef struct TASK_PARAM
{
    int64_t ItemId;
    BYTE ParamFlags;
    BYTE Reserv;
    RPARAM InitValue;
} )
 TASK_PARAM;



#endif // #ifndef TASK_H
