#pragma once
#include <boost/predef.h>

#ifdef _WIN32
// error 12312
#    define MPLC_FUNCTION_NAME() __FUNCTION__
#else
#    define MPLC_FUNCTION_NAME() __PRETTY_FUNCTION__
#endif
