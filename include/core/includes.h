#ifndef INCLUDES_H
#define INCLUDES_H

#include "config.h"
#if defined(QNX_NEUTRINO) // QNX neutrino не поддерживает вложеные extern "C" http://www.qnx.com/support/knowledgebase.html?id=50130000000Pbr6
	#include <math.h>
	#include <float.h>
#endif

#include "mplc_stdint.h" // For replace all current millions names of int* types

BEGIN_EXTERN_C

// Попытка привести код подобно http://stackoverflow.com/questions/1537964/visual-c-equivalent-of-gccs-attribute-packed
#if defined(LINUX) || defined(ECOS)
	#define _PACKED( ... ) __VA_ARGS__ __attribute__((packed))
	#define _PACKED_BEGIN 
	#define _PACKED_END 
	#define R_PACKED __attribute__((packed))
#elif defined(_WIN32) || defined(WINCE) || defined(_WIN64)
	#define _PACKED( ... ) __pragma( pack(push, 1) ) __VA_ARGS__ __pragma( pack(pop) )
	#define _PACKED_BEGIN __pragma ( pack(push, 1) )
	#define _PACKED_END __pragma ( pack(pop) )
	#define R_PACKED
#else
	#error platform type
#endif



#if !defined(QNX_NEUTRINO) // QNX neutrino не поддерживает вложеные extern "C" http://www.qnx.com/support/knowledgebase.html?id=50130000000Pbr6
	#include <math.h>
	#include <float.h>
#endif

#if defined(TREI_EMUL)
    #include "drivers/trei/trai_9xx_win_emul.h"
#endif

#if defined(WINCE)
    #include <stdlib.h>
    #include <stdio.h>
#ifndef _WINSOCK2API_
	#define WIN32_LEAN_AND_MEAN 
	#include <windows.h>	
	#include <winsock2.h>
#endif
//#undef IN
	//#undef OUT
    typedef HANDLE RHANDLE;
    #define PTR *UNALIGNED
	#define itoa	_itoa
	#define vsnprintf	_vsnprintf
	#define R_CRITICAL_SECTION	CRITICAL_SECTION
# define inline __inline
	#define nullptr NULL

	char * _i64toa(int64_t __val, char *__string, int __radix);
	char * _ui64toa(uint64_t __val, char *__string, int __radix);

#elif defined(_WIN32)
    #include <io.h>
    #include <stdlib.h>
    #include <stdio.h>
	#include <string.h>
    #include <fcntl.h>
    
	#include <winsock2.h>
	#include <windows.h>
	#include <tchar.h>
	#include <sys/stat.h>
    #include <process.h>
	//#undef IN
	//#undef OUT
    typedef HANDLE RHANDLE;
	#define _CRT_SECURE_NO_DEPRECATE
	#undef _SECURE_SCL
	#define _SECURE_SCL	0
	#define R_CRITICAL_SECTION	CRITICAL_SECTION
#if _MSC_VER < 1600
	#define nullptr NULL
#endif
#if defined(_WIN64)
#	   define TARGET_PLATFORM_NAME "win64"
#else
#	   define TARGET_PLATFORM_NAME "win32"
#    endif

#elif defined(LINUX)
    #include <stdlib.h>
    #include <stdio.h>
	#include <string.h>
    #include <fcntl.h>
    #include <pthread.h>
	#include <errno.h>
	#include <sys/stat.h>
	#include <sys/socket.h> //For fionread cygwin
	#include <unistd.h>
	#include <semaphore.h>
	#include <sys/types.h>
    typedef pthread_t RHANDLE;
    #define R_CRITICAL_SECTION	pthread_mutex_t
    #define _stat stat
//    typedef int HANDLE;
   
	#ifndef _FILETIME_
		typedef struct _FILETIME {
			DWORD dwLowDateTime;
			DWORD dwHighDateTime;
		} FILETIME, *PFILETIME, *LPFILETIME;
		#define _FILETIME_
	#endif

	

#if !defined(S_OK)
	#define S_OK                                   ((HRESULT)0L)
#endif

#if !defined(S_FALSE)
	#define S_FALSE                                ((HRESULT)1L)
#endif


#if !defined(QNX_NEUTRINO)
	extern void itoa(int i, char *buf, int len);
	EXTERN_C int gettid(void);
#endif
	#define _atoi64 atoll

	#define ZeroMemory(arg0,arg1) memset(arg0,0,arg1);
	#define GetCurrentThreadId() gettid()

#elif defined(ECOS)
    #include <stdlib.h>
    #include <stdio.h>
    #include <time.h>
    #include <cyg/kernel/kapi.h>
	#include <errno.h>
    #define DEF_THREAD_PRI	10
    typedef struct {
	cyg_handle_t	handle;
	cyg_thread	obj;
	char		name[16];
	char		stack[0];
    } THREAD_DATA;
    typedef THREAD_DATA* RHANDLE;
	typedef char TCHAR;
	#define R_CRITICAL_SECTION	cyg_mutex_t

#else
    #error Build type not defined !!!
#endif

//Addition defines
#ifndef PTR
	#define PTR *
#endif

#define R_HUGE_PTR PTR

#if !defined(WINCE) && !defined(UNALIGNED)
    #define UNALIGNED
#endif

#ifndef R_CRITICAL_SECTION
	#define R_CRITICAL_SECTION	int
#endif

//Типы значений
typedef DWORD RBOOLEAN;
typedef int RINTEGER;
#ifdef PLC_8BYTE_PARAM
#define SIZE_PARAM_VALUE 8
typedef double RFLOAT;
#else
#error Not supported 
#endif
typedef float RFLOAT4B;


#ifdef BENDIAN
	#define PLC_CONV_NUMBERS
	extern WORD WordSwap( WORD s );
	extern DWORD DwordSwap (DWORD i);
	extern float FloatSwap( float f );
#elif defined(PLC100) || defined(PLC_LINPAC) //VictorM Как выяснилось, в последних прошивках PLC304 не надо переворачивать double
	#define PLC_CONV_NUMBERS
	#define WordSwap(s) (s)
	#define DwordSwap(s) (s)
	#define FloatSwap(s) (s)
#endif

#ifdef PLC_CONV_NUMBERS
	extern double DoubleSwap( double f );
#endif

//for PLCOWEN
#if defined(TKM410) || defined(NEEDALIGNED)
static inline WORD _get_unaligned_word(BYTE* p)
{
    return p[0] | p[1] << 8;
}
static inline DWORD _get_unaligned_dword(BYTE* p)
{
    return p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;
}

static inline QWORD _get_unaligned_qword(BYTE* p)
{
	return ((QWORD)((p[4]) | (p[5] << 8) | (p[6] << 16) | (p[7] << 24)) << 32) | (DWORD)(p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24);
	//return p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24 | p[4] << 32 | p[5] << 40 | p[6] << 48 | p[7] << 56;
}


static inline void _put_unaligned_word(register BYTE* p, DWORD v)
{
    *p++ = v;
    *p++ = v >> 8;
}

static inline void _put_unaligned_dword(register BYTE* p, DWORD v)
{
    _put_unaligned_word(p + 2, v >> 16);
    _put_unaligned_word(p, v);
}

static inline void _put_unaligned_qword(register BYTE* p, QWORD v)
{
	_put_unaligned_word(p + 6, v >> 48);
	_put_unaligned_word(p + 4, v >> 32);
	_put_unaligned_word(p + 2, v >> 16);
	_put_unaligned_word(p, v);
}


    #define GET_UNALIGNED_WORD(ptr)	_get_unaligned_word((BYTE*)(ptr))
    #define GET_UNALIGNED_DWORD(ptr)	_get_unaligned_dword((BYTE*)(ptr))
	#define GET_UNALIGNED_QWORD(ptr)	_get_unaligned_qword((BYTE*)(ptr))
    #define GET_UNALIGNED_RINTEGER	GET_UNALIGNED_DWORD

static inline float GET_UNALIGNED_RFLOAT4B(BYTE PTR ptr) {
	union
	{
		float f;
		unsigned char b[4];
	} dat1;

	memcpy(dat1.b, ptr, 4);
	return dat1.f;
//#ifdef PLC_CONV_NUMBERS
//	return FloatSwap(dat1.f);
//#else
//	return dat1.f;
//#endif
}



/// @todo Отдельно надо разбиратся с переопределением типов, выравниванием и прочей платформозависимой ерундой.
#if !defined(PLC100) && !defined(PLC_LINPAC)
extern float FloatSwap(float f);
#endif

static inline void PUT_UNALIGNED_RFLOAT4B(register BYTE* p, RFLOAT4B v) {

	union
	{
		float f;
		unsigned char b[4];
	} dat1;

#ifdef PLC_CONV_NUMBERS
	dat1.f = FloatSwap(v);
#else
	dat1.f = v;
#endif
	memcpy(p,dat1.b,4);
}


	#define GET_UNALIGNED_WORD_FAR	GET_UNALIGNED_WORD
    #define GET_UNALIGNED_WORD_HUGE	GET_UNALIGNED_WORD
    #define GET_UNALIGNED_DWORD_FAR	GET_UNALIGNED_DWORD
    #define GET_UNALIGNED_DWORD_HUGE	GET_UNALIGNED_DWORD

    #define PUT_UNALIGNED_WORD(ptr,val)	 _put_unaligned_word((BYTE*)(ptr),(val))
    #define PUT_UNALIGNED_DWORD(ptr,val) _put_unaligned_dword((BYTE*)(ptr),(val))
	#define PUT_UNALIGNED_QWORD(ptr,val) _put_unaligned_qword((BYTE*)(ptr),(val))

#elif BENDIAN

static inline WORD _get_unaligned_word(BYTE* p)
{
    return p[0] | p[1] << 8;
}
static inline DWORD _get_unaligned_dword(BYTE* p)
{
    return p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;
}

static inline void _put_unaligned_word(register BYTE* p, DWORD v)
{
    *p++ = v;
    *p++ = v >> 8;
}

static inline void _put_unaligned_dword(register BYTE* p, DWORD v)
{
    _put_unaligned_word(p + 2, v >> 16);
    _put_unaligned_word(p, v);
}

    #define GET_UNALIGNED_WORD(ptr)	_get_unaligned_word((BYTE*)(ptr))
    #define GET_UNALIGNED_DWORD(ptr)	_get_unaligned_dword((BYTE*)(ptr))
    #define GET_UNALIGNED_RINTEGER	GET_UNALIGNED_DWORD
    #define GET_UNALIGNED_RFLOAT4B	GET_UNALIGNED_DWORD
    #define GET_UNALIGNED_WORD_FAR	GET_UNALIGNED_WORD
    #define GET_UNALIGNED_WORD_HUGE	GET_UNALIGNED_WORD
    #define GET_UNALIGNED_DWORD_FAR	GET_UNALIGNED_DWORD
    #define GET_UNALIGNED_DWORD_HUGE	GET_UNALIGNED_DWORD

    #define PUT_UNALIGNED_WORD(ptr,val)	 _put_unaligned_word((BYTE*)(ptr),(val))
    #define PUT_UNALIGNED_DWORD(ptr,val) _put_unaligned_dword((BYTE*)(ptr),(val))

#else
    #define GET_UNALIGNED_WORD(ptr)	(*(UNALIGNED WORD*)(ptr))
    #define GET_UNALIGNED_DWORD(ptr) (*(UNALIGNED DWORD*)(ptr)) 
	#define GET_UNALIGNED_QWORD(ptr)	(*(UNALIGNED QWORD*)(ptr))
    #define GET_UNALIGNED_WORD_FAR(ptr)	(*(UNALIGNED WORD PTR)(ptr))
    #define GET_UNALIGNED_WORD_HUGE(ptr)	(*(UNALIGNED WORD R_HUGE_PTR)(ptr))
    #define GET_UNALIGNED_DWORD_FAR(ptr)	(*(UNALIGNED DWORD PTR)(ptr))
    #define GET_UNALIGNED_DWORD_HUGE(ptr)	(*(UNALIGNED DWORD R_HUGE_PTR)(ptr))
    #define GET_UNALIGNED_RINTEGER(ptr)	(*(UNALIGNED RINTEGER*)(ptr))
    #define GET_UNALIGNED_RFLOAT(ptr)	(*(UNALIGNED RFLOAT*)(ptr))
    #define GET_UNALIGNED_RFLOAT4B(ptr)	(*(UNALIGNED RFLOAT4B*)(ptr))

    #define PUT_UNALIGNED_WORD(ptr,val)	 (*(UNALIGNED WORD*)(ptr) = (val))
    #define PUT_UNALIGNED_DWORD(ptr,val) (*(UNALIGNED DWORD*)(ptr) = (val))
	#define PUT_UNALIGNED_QWORD(ptr,val) (*(UNALIGNED QWORD*)(ptr) = (val))
	#define PUT_UNALIGNED_RFLOAT4B(ptr,val)	(*(UNALIGNED RFLOAT4B*)(ptr) = (val))
#endif

#if defined(TKM410) || defined(NEEDALIGNED) || defined(BENDIAN)
	extern double GET_UNALIGNED_DOUBLE(BYTE* ptr);
	extern void PUT_UNALIGNED_DOUBLE(BYTE* ptr, double val);
#else
	#define GET_UNALIGNED_DOUBLE(ptr)	(*(UNALIGNED double*)(ptr))
	#define PUT_UNALIGNED_DOUBLE(ptr,val) (*(UNALIGNED double*)(ptr) = (val))
#endif


#if defined(_WIN64) || (__WORDSIZE == 64) 
#define PUT_UNALIGNED_PTR(ptr,val)	PUT_UNALIGNED_QWORD(ptr,(QWORD)val)
#define GET_UNALIGNED_PTR(ptr)	(BYTE*)GET_UNALIGNED_QWORD(ptr)
#else 
#define PUT_UNALIGNED_PTR(ptr,val)	PUT_UNALIGNED_DWORD(ptr,(DWORD)val)
#define GET_UNALIGNED_PTR(ptr)	(BYTE*)GET_UNALIGNED_DWORD(ptr)
#endif


#ifndef NULL
#ifdef __cplusplus
	#define NULL    0
#else
	#define NULL    ((void *)0)
#endif
#endif

#if defined(LINUX)
	#define GetLastError()	errno
#elif !defined(_WIN32)
	#define GetLastError()	0
#endif
#define RGetLastError()	GetLastError()

#define RESCHEDULE()	RSleep_ms(0)
#define FAST_RESCHEDULE()

extern BYTE SilenceMode;

#if !defined(NO_MAX_OPTIMIZATION) || !defined(USE_DISPLAY) || defined(NOT_USE_DISPLAY)
  #define PRINT_NEW_LINE()
  #define PRINT(...)
  #define PRINTLN(...)
#elif defined(USE_TELNET)
  EXTERN_C PRINTF_ATTR(1, 2) int logMsg(_Printf_format_string_ const char* fmt, ... );
  EXTERN_C PRINTF_ATTR(1, 2) int logMsgS(_Printf_format_string_ const char* s);
  EXTERN_C PRINTF_ATTR(1, 2) int logMsgLn(_Printf_format_string_ const char* fmt, ...);
  #define PRINT_NEW_LINE() logMsg("\r\n")
  #define PRINT(...) logMsg(__VA_ARGS__)
  #define PRINTLN(...) logMsgLn(__VA_ARGS__)
#else
	#define PRINT_NEW_LINE() printf("\n")
	#define PRINT(...) printf(__VA_ARGS__)
	#define PRINTLN(...) do { printf(__VA_ARGS__); } while(0)
#endif

#ifdef _WIN32
//	#undef S_OK
	#undef E_FAIL
	#undef E_POINTER
	#undef E_INVALIDARG
#endif

#ifndef _WIN32
	#define _tfopen fopen
	#define _T(s) s
	#define _tcscpy strcpy
	#define _tcsncpy strncpy
	#define _tcschr strchr
	#define _stprintf sprintf
	#define _tcscat strcat
	#define _tcscspn strcspn
	#define PATH_DELIM "/"
#else
#if defined PC_WIN32_ONLY
	#define PATH_DELIM "\\"
#else 
#define PATH_DELIM "/"
#endif
#endif

#ifndef MAX_PATH
	#define MAX_PATH          260
#endif
#define MAX_FILENAME_LEN MAX_PATH


#ifndef MAXINT16
#define MAXINT16     0x7fff
#endif
#ifndef MININT16     
#define MININT16     (-MAXINT16)
#endif
#ifndef MAXUINT16
#define MAXUINT16    0xffff
#endif

#ifndef MAXINT32
#define MAXINT32     0x7fffffff
#endif
#ifndef MININT32     
#define MININT32     (-MAXINT32)
#endif
#ifndef MAXUINT32    
#define MAXUINT32 0xffffffff
#endif
#ifndef MAXDWORD
#define MAXDWORD MAXUINT32
#endif
#ifndef MAXINT64
#define MAXINT64     0x7fffffffffffffffLL
#endif

#ifndef MAXLONG64
#define MAXLONG64 0x7fffffffffffffffLL
#endif
#ifndef MINLONG64 
#define    MINLONG64 (-MAXLONG64)
#endif
#ifndef MAXULONG64 
#define MAXULONG64 0xffffffffffffffffULL
#endif

END_EXTERN_C

#endif // #ifndef INCLUDES_H
