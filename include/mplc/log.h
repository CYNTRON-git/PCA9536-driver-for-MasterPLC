#pragma once
#include "config.h"

BEGIN_EXTERN_C
extern int PRINTF_ATTR(2, 3) logMsgInf(int setLevel, _Printf_format_string_ const char* fmt, ...);
END_EXTERN_C

//константы взяты из макросов в <opcua/opcua_trace.h>
#define PRINT_INFO(...) logMsgInf(0x00000004, __VA_ARGS__)
#define PRINT_TRACE(...) logMsgInf(0x00000002, __VA_ARGS__)
#define PRINT_WARN(...) logMsgInf(0x00000010, __VA_ARGS__)
#define PRINT_SYS(...) logMsgInf(0x00000008, __VA_ARGS__)
#define PRINT_ERROR(...) logMsgInf(0x00000020, __VA_ARGS__)
