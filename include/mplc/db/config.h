#pragma once

#ifdef WIN32
#    ifdef MPLC_DATABASE_EXPORTS
#        define MPLC_DATABASE_API __declspec(dllexport)
#    else
#        define MPLC_DATABASE_API __declspec(dllimport)
#    endif
#else
#    define MPLC_DATABASE_API
#endif
