#pragma once

#ifdef _WIN32
#    ifdef MPLC_LOCL_STORAGE_EXPORTS
#        define MPLC_LOCL_STORAGE_API __declspec(dllexport)
#    else
#        define MPLC_LOCL_STORAGE_API __declspec(dllimport)
#    endif
#else
#    define MPLC_LOCL_STORAGE_API
#endif


