#pragma once

#define REQUEST_UDP	0
#define REQUEST_JSON 1
#define REQUEST_GET_PARAM 2
#define REQUEST_PRINT 3
#define REQUEST_JSON_STREAM 4

#define SERVER_PARAM_ETH_ADDRESS "ethAddress"
#define SERVER_PARAM_FAST_CGI_PORT "fastcgiPort"
#define SERVER_PARAM_PROJECT_ID "projectId"
#define SERVER_PARAM_PROJECT_SESSION_ID "projectSessionId"
#define SERVER_PARAM_SESSIONS_LIMIT "SessionsLimit"
#define SERVER_PARAM_STATUS "status"
#define SERVER_PARAM_START_ERROR_TEXT "startErrorText"
#define SERVER_PARAM_START_ERROR "startError"
#define SERVER_PARAM_RT_VERSION_DATE "rtVersionDate"

#define SERVER_STATUS_IS_MASTER	0x4   //Контроллер является мастером

#ifndef EXTERN_C
	#ifdef __cplusplus
	#define EXTERN_C    extern "C"
	#else
	#define EXTERN_C    extern
	#endif
#endif

#if (defined(_WIN32) || defined(_WIN64)) && !defined(MASTER_PLC_LIB)
#   ifdef MPLC_ADDIN_EXPORTS
#       define MPLC_ADDIN_API  __declspec(dllexport)
#       define MPLC_SERVER_API __declspec(dllimport)
#   else
#       define MPLC_ADDIN_API  __declspec(dllimport)
#       define MPLC_SERVER_API __declspec(dllexport)
#   endif
#else
#   define MPLC_ADDIN_API 
#   define MPLC_SERVER_API 
#endif


//Addin callbacks
typedef int(*ProcessRequestCallback)(int nRequestType, const void* pRequest, int nRequestSize, void* pResponse, int nMaxResponseSize, int* pnResponseSize);
EXTERN_C MPLC_ADDIN_API int InitAddin(ProcessRequestCallback func, int nInFlags, int* pnOutFlags);
EXTERN_C MPLC_ADDIN_API void DisposeAddin();

//Server methods
typedef int(*RequestProcessorCallback)(void* data, int nRequestType, const void* pRequest, int nRequestSize, void* pResponse, int nMaxResponseSize, int* pnResponseSize);
EXTERN_C MPLC_SERVER_API int RegisterRequestProcessor(int nRequestType, RequestProcessorCallback func, void* data, void** node);
EXTERN_C MPLC_SERVER_API int UnRegisterRequestProcessor(void* ptr);
//Config events addins
typedef enum ConfigProcessorMode
{
    ConfigProcessorInit,
    ConfigProcessorAfterWsCtxInit,
    ConfigProcessorAfterWsHostsInit,
    ConfigProcessorAfterWsProtocolsInit,
    ConfigProcessorLoadVMInfo,
    ConfigProcessorAfterLoadedVMInfo,
    ConfigProcessorLoadedDatabase,
	ConfigProcessorLoaded,
	ConfigProcessorBeforeInitTasks,
	ConfigProcessorAfterInitTasks,
	ConfigProcessorBeforeStartTasks,
	ConfigProcessorAfterStartTasks,
	ConfigProcessorBeforeStopTasks,
    ConfigProcessorAfterStopTasks,
    ConfigProcessorStopped,
    ConfigProcessorAfterStopped,
    ConfigProcessorBeforeWsCtxStop,
    ConfigProcessorBeforeWsCtxClear,
	ConfigProcessorDone,
} ConfigProcessorMode;

struct ControllerConfig;
typedef int(*ConfigProcessorCallback)(void* data, ConfigProcessorMode mode, ControllerConfig *config);
EXTERN_C MPLC_SERVER_API int RegisterConfigProcessor(const char* name, unsigned int modeMask, ConfigProcessorCallback func, void* data, int priority, int* pnIndex);

EXTERN_C void OnAddinEvent(int addinID, int event, const char* curRequest);
