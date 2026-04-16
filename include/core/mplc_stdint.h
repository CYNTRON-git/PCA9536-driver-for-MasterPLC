#ifndef __MPLC_STDINT_H
#define __MPLC_STDINT_H
#define HAVE_STDINT_H 1  // For net-snmp-config.h

#undef BOOL
#undef DATE
#undef UINT
#undef INT
#undef DOUBLE
#undef ULONGLONG
#undef LONGLONG
#undef SHORT
#undef BOOL
#undef BOOLEAN
#undef INT64
#undef INT
#undef UINT
#undef LONG
#undef ULONG

#ifdef _WIN32
#    include "win/inttypes.h"
#else
#    include "linux/inttypes.h"
#endif
//#include <pstdint.h>

typedef unsigned short WORD;
typedef unsigned char BYTE;

#if defined(LINUX)
#    if __WORDSIZE == 64  // InSAT для 64-х бит linux
typedef unsigned int DWORD;
#    else
typedef unsigned long DWORD;
#    endif
typedef uint64_t QWORD;
#else
typedef unsigned long DWORD;
typedef unsigned __int64 QWORD;
#endif
typedef double DATE;

#include <stdint.h>
#endif  //__MPLC_STDINT_H
