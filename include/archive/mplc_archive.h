// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the MPLC_ARCHIVE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// MPLC_ARCHIVE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef WIN32
#    ifdef MPLC_ARCHIVE_EXPORTS
#        define MPLC_ARCHIVE_API __declspec(dllexport)
#    else
#        define MPLC_ARCHIVE_API __declspec(dllimport)
#    endif
#else
#    define MPLC_ARCHIVE_API
#endif

#include "archive/archive_share.h"
#include "archive_subscription.h"
#include "archive/archive_request_processor.h"
#include "share/addin_base.h"
#include "share/base_source.h"
#include "archive/archive_source.h"
#include "archive/archive_addin.h"
