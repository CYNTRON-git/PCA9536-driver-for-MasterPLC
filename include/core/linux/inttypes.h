#ifndef __LINUX_INTTYPES_H
#define __LINUX_INTTYPES_H
#ifndef _MSC_VER

#include <stdint.h>

#undef __int64
#undef __int32
#undef __int16
#undef __int8

typedef char TCHAR;
//
//typedef signed char             int8_t;
//typedef short                   int16_t;
//typedef int                     int32_t;
//typedef signed long long int    int64_t;
//typedef unsigned char           uint8_t;
//typedef unsigned short          uint16_t;
//typedef unsigned int            uint32_t;
//typedef unsigned long long int  uint64_t;
//
//typedef signed char        int_least8_t;
//typedef short              int_least16_t;
//typedef int                int_least32_t;
//typedef int64_t            int_least64_t;
//typedef unsigned char      uint_least8_t;
//typedef unsigned short     uint_least16_t;
//typedef unsigned int       uint_least32_t;
//typedef uint64_t           uint_least64_t;
//
//#ifndef QNX_NEUTRINO
//    typedef signed char        int_fast8_t;
//    typedef int                int_fast16_t;
//    typedef int                int_fast32_t;
//    typedef long long          int_fast64_t;
//    typedef unsigned char      uint_fast8_t;
//    typedef unsigned int       uint_fast16_t;
//    typedef unsigned int       uint_fast32_t;
//    typedef unsigned long long uint_fast64_t;
//#endif

//typedef long long __int64;
//typedef int __int32;
//typedef short __int16;
//typedef char __int8;
typedef unsigned long long ULONGLONG;
typedef long long LONGLONG;
typedef int64_t INT64;
typedef uint8_t BOOLEAN;
typedef int INT;
typedef uint32_t UINT;

typedef double DOUBLE;
typedef void* HCERTSTORE;
# if __WORDSIZE == 64 
	typedef int HRESULT;
#else
	typedef long HRESULT;
#endif

typedef int BOOL;
typedef short SHORT;

typedef long LONG;
typedef unsigned long ULONG;
typedef const char* LPCSTR;

#endif  //_MSC_VER
#endif      // __LINUX_INTTYPES_H
