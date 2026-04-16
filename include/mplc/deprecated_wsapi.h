#pragma once

#ifdef _WIN32
#    ifdef MPLC_WS_EXPORTS
#        define MPLC_WS_API __declspec(dllexport)
#    else
#        define MPLC_WS_API __declspec(dllimport)
#    endif
#else
#    define MPLC_WS_API
#endif

#include "deprecated_ws/ws_addin.h"
#include "deprecated_ws/ws_connect.h"
#include "deprecated_ws/ws_server.h"
#include "deprecated_ws/ws_data.h"
