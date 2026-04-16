#ifndef STTime_h
#define STTime_h
#include <mplc_stdint.h>

EXTERN_C FILETIME luaST_checkFT(lua_State *L, int n);
EXTERN_C int newFTfromFT(lua_State *L, FILETIME ft);
EXTERN_C int newFTfromRDateTime(lua_State *L, RDateTime t);
EXTERN_C int String_DT_2_RTIME_61131(const char* str, size_t len, RTIME* rtime, int* out_nsec);
EXTERN_C int String_TIME_2_RTIME_61131(const char* str, struct RTIME* ft, int* out_nsec);
EXTERN_C int String_DATE_2_RTIME_61131(const char* str, struct RTIME* ft);

EXTERN_C int luaopen_FT(lua_State* L);
#define FileTimeName "FileTime"
#define FileTimeLib "FILETIME"

#endif
