#pragma once
#include <core/config.h>
#include <mplc_stdint.h>
BEGIN_EXTERN_C
extern size_t utf8_to_cp1251(const char* utf8, char* windows1251, size_t n);
extern size_t cp1251_to_utf8_diff(const char* in, size_t len);
extern size_t cp1251_to_utf8(char* out, const char* in, size_t len);
END_EXTERN_C
