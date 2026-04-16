#pragma once

#ifndef MPLCCOMMON_API
#    if defined(_WIN32) || defined(WINCE) || defined(X64) || defined(_WIN64)
#        ifdef MPLCCOMMON_EXPORTS
#            define MPLCCOMMON_API __declspec(dllexport)
#        else
#            define MPLCCOMMON_API __declspec(dllimport)
#        endif
#    else
#        define MPLCCOMMON_API
#    endif
#endif

