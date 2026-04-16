#pragma once
// InSAT В оригинальной версии 1.04 общего файла opcua_platformdefs.h нет. Есть только отдельные файлыдля каждой платформы.
// Возможно нужно объеденить opcua_platformdefs.h для разных платформ в один файл

#pragma once

#ifdef _GNUC_
#include <wchar.h>
#include <platforms/linux/opcua_platformdefs.h> /* includes typemapping of primitives */
#else
#include <platforms/win32/opcua_platformdefs.h> /* includes typemapping of primitives */
#endif


#ifndef OPCUA_SUPPORT_PKI
#define OPCUA_SUPPORT_PKI                           OPCUA_CONFIG_YES
#endif  /* OPCUA_SUPPORT_PKI */

#if OPCUA_SUPPORT_PKI
#define OPCUA_SUPPORT_PKI_OVERRIDE                  OPCUA_CONFIG_YES
#define OPCUA_SUPPORT_PKI_OPENSSL                   OPCUA_CONFIG_YES
	#if defined(WIN32)
		#ifdef _WIN32_WCE
		#define OPCUA_SUPPORT_PKI_WIN32                     OPCUA_CONFIG_NO
		#else
		#define OPCUA_SUPPORT_PKI_WIN32                     OPCUA_CONFIG_YES
		#endif
	#endif 
#endif /* OPCUA_SUPPORT_PKI */


// InSAT Взято из пред версии. в 1.04 такого нет.


/* Detect endianess of this unix system */
// #ifdef _GNUC_
	// #if defined(__sun) || defined(sun)
		// # define BIG_ENDIAN 4321
		// # define LITTLE_ENDIAN 1234
	// # if defined(_BIG_ENDIAN)
		// #  define BYTE_ORDER BIG_ENDIAN
	// # else
		// #  define BYTE_ORDER LITTLE_ENDIAN
	// # endif
	// #else
		// # include <endian.h>
	// #endif
	// #include <stdio.h>
// #else  // InSAT for WIN32
	// #  define BYTE_ORDER LITTLE_ENDIAN
// #endif


/* basic type mapping */
#include "platforms/opcua_p_types.h"


/* shortcuts to OpcUa_String functions */
#define OpcUa_StrLen(xStr)                            OpcUa_String_StrLen(xStr)

#define OpcUa_StrCpy(xDst, xSrc)                      OpcUa_String_StrnCpy(xDst, xSrc, OPCUA_STRING_LENDONTCARE)
#define OpcUa_StrnCpy(xDst, xDstLength, xSrc, xCount) OpcUa_String_StrnCpy(xDst, xDstLength, xSrc, xCount)

#define OpcUa_StrCat(xDst, xSrc)                      OpcUa_String_StrCat(xDst, xSrc, OPCUA_STRING_LENDONTCARE)
#define OpcUa_StrnCat(xDst, xDstLength, xSrc, xCount) OpcUa_String_StrnCat(xDst, xDstLength, xSrc, xCount)


// Возможно надо перетащить в opcua_string.h дефайны для строк
#ifdef _GNUC_
		typedef void* LPVOID;
		#define OpcUa_SWPrintf								swprintf
#else
	/* NOT _GNUC_ CODE */
	#if OPCUA_USE_SAFE_FUNCTIONS
	    #define OpcUa_StrnCpyA(xDst, xDstSize, xSrc, xCount) strncpy_s(xDst, xDstSize, xSrc, xCount)
	    #define OpcUa_StrnCatA(xDst, xDstSize, xSrc, xCount) strncat_s(xDst, xDstSize, xSrc, xCount)
	    #define OpcUa_SPrintfA                               sprintf_s
	    #define OpcUa_SnPrintfA                              _snprintf_s
	    #define OpcUa_SScanfA                                sscanf_s
        #ifndef _WIN32_WCE
            #define OpcUa_SWPrintf								swprintf_s
        #endif
	#else
	    #define OpcUa_StrnCpyA(xDst, xDstSize, xSrc, xCount) strncpy(xDst, xSrc, xCount)
	    #define OpcUa_StrnCatA(xDst, xDstSize, xSrc, xCount) strncat(xDst, xSrc, xCount)
	    #define OpcUa_SPrintfA                               sprintf
	    #define OpcUa_SnPrintfA                              _snprintf
	    #define OpcUa_SScanfA                                sscanf
	    #define OpcUa_SScanfA                                sscanf
        #ifndef _WIN32_WCE
	        #define OpcUa_SWPrintf								swprintf
        #endif
	#endif
#endif

/* DLL and function handling*/
#if defined(WIN32) || defined(_WIN32_WCE)
	#define OpcUa_GetProcAddress							GetProcAddress
#else
	#ifdef _GNUC_
		#define OpcUa_GetProcAddress								dlsym
	#endif
#endif

#ifdef _WIN32_WCE
	#define OpcUa_Clock										GetTickCount
#else
	#define	OpcUa_Clock										clock
#endif



// End InSAT Взято из пред версии. в 1.04 такого нет.
