#pragma once

#ifndef MPLC_AGGREAGATION_API
#    if defined(WIN32)
#        ifdef MPLC_AGGREAGATION_EXPORTS
#            define MPLC_AGGREAGATION_API __declspec(dllexport)
#        else
#            define MPLC_AGGREAGATION_API __declspec(dllimport)
#        endif
#    else
#        define MPLC_AGGREAGATION_API
#    endif
#endif
