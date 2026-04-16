#pragma once

#ifndef MPLCSHARE_API
#    if defined(_WIN32) || defined(WINCE) || defined(X64) || defined(_WIN64)
#        ifdef MPLCSHARE_EXPORTS
#            define MPLCSHARE_API __declspec(dllexport)
#        else
#            define MPLCSHARE_API __declspec(dllimport)
#        endif
#    else
#        define MPLCSHARE_API
#    endif
#endif

