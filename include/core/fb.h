
#ifndef FB_H
#define FB_H

#include "main.h"

#define GUARD_VALUE 0x12345678
// --------------------------------------------------------
// FB Mode
// --------------------------------------------------------
typedef enum FB_MODE {
    fbmGetMem,
    fbmInit,
    fbmExecute,
    fbmInitType,
    fbmCheckMemory,
    fbmPostExecute
} FB_MODE;

enum FB_RUNMASK { fbRunAtStart = 0x1, fbRunAtEnd = 0x2 };

enum FB_FLAGS { fbInited = 0x1, fbInitFailed = 0x2, fbNeedPostExecute = 0x4, fbUseCodeEncryption = 0x10 };

_PACKED(typedef struct FB_HEADER {
    WORD CfgSize;
    WORD Number;
    WORD Multiple;
    WORD Type;
    BYTE Flags;
    BYTE RunMask;
    WORD QuanParams;
    WORD NumFirstControl;
})
FB_HEADER;

_PACKED_BEGIN

typedef struct FB_CB {
    UNALIGNED FB_HEADER PTR pHeader;
    UNALIGNED WORD PTR pListParams;
    BYTE PTR pConfig;
    //BYTE PTR pShareMem;           // pointer to FB sharing memory
    //BYTE PTR pPersonalMem;        // pointer to FB personal memory
    //BYTE PTR pLoadedPersonalMem;  // pointer to FB personal memory, loaded from hotrestart
    DWORD dwGuard1;
    void PTR pParentTask;   // pointer to parent task (FBEXEC_CB)
    //DWORD SizeShareMem;     // size FB sharing memory
    WORD MultipleCount;
    BYTE InUse;
    BYTE bRTFlags;
#ifdef MPLC_MANAGED_SUPPORT
    void* pvHost;
#endif
} R_PACKED FB_CB;

_PACKED_END

typedef int (*pFB_proc)(FB_MODE fbMode, FB_CB PTR fbCB);

extern DWORD GetFBConfigSize(FB_CB PTR fbCB);
extern DWORD GetCfgSize(FB_HEADER PTR pHeader);
extern int GetFBEstimatedPeriod(FB_CB PTR fbCB);
extern void PrintRPARAM(RPARAM PTR pParam);

// --------------------------------------------------
// Functions for writing port drivers
// --------------------------------------------------

// return	0 - no request,
//			1 - low priority request
//			2 - high priority request
typedef int (*PortDriverFuncGetRequestPriority)(void* pData);

// Always return 0;
typedef int (*PortDriverFuncOnRestart)(void* pData);
    // nTimeout - in msec

// nResult 0 - success
//		  -2 - timeout
// return 0 - normal
//		 1 - has another request
typedef int (*PortDriverFuncOnReply)(void* pData, char* szReply, int nReplyLength, int nResult);

typedef struct PortDriverRequestData {
    RINTEGER nTimeout;
    char cEndReplyChar;
    int maxResponseCount;
} PortDriverRequestData;

typedef int (*PortDriverFuncGetRequest)(void* pData,
                                        char* szRequest,
                                        PortDriverRequestData PTR pRequestData,
                                        PortDriverFuncOnReply PTR funcOnReply);

// Called from fbmGetMem
// 0 - success
EXTERN_C int PortDriverAllocate(int nPort);

// Called from fbmInit
// pData - fbCB
// 0 - success
EXTERN_C int PortDriverRegister(int nPort,
                                void* pData,
                                PortDriverFuncGetRequestPriority funcGetRequestPriority,
                                PortDriverFuncGetRequest funcGetRequest,
                                int* pnDriverIndex);

extern int PortDriverWrite(int nPort, char* szData, int nDataLength);

//
extern int matherr_code;

#endif  // #ifndef FB_H
