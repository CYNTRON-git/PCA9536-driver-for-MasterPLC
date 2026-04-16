#pragma once
#include "fn_name.h"
#ifdef _WIN32
#    define MPLC_NOT_IMPLEMENTED(Cond) static_assert((Cond), "NOT IMPLEMENTED: " MPLC_FUNCTION_NAME())
#else
#    define MPLC_NOT_IMPLEMENTED(Cond)  //static_assert((Cond), "NOT IMPLEMENTED")
#endif
