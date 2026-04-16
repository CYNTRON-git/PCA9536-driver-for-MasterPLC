
#ifndef RESERV_H
#define RESERV_H

#include "communic/netlib.h"
#include "main_imp.h"
#ifdef PC_MSDOS
#    define RESERV_RECV_BUFF_SIZE 1000
#else
#    define RESERV_RECV_BUFF_SIZE (500 * sizeof(RPARAM))  // 5000
#endif

#define RESERV_SEND_BUFF_SIZE 100

typedef struct RESERV_TASK_STAT {
    RBOOLEAN SyncSucceeded;
    int SuccessfulSyncCount;
    int SyncDataSize;
    int SyncErrorsCount;
    RDateTime LastSyncTime;
    int64_t LastSyncDuration;
} RESERV_TASK_STAT;

typedef struct RESERV_TASK_CB {
    UNALIGNED TASK_HEADER PTR pHeader;
    WORD ProtocolType;
    WORD Flags;
    NLIPAddressList IPAddressList;
    DWORD dwTimeout;
    DWORD ControllerAddress;
    struct NLSocket MySocket;
    RESERV_TASK_STAT Stat;

    DWORD LenLastStep_msec;
    DWORD LenLastSync_msec;
    DWORD StartLastStep_msec;
    DWORD cicle_counter;
    //DWORD sync_counter;
    //DWORD error_counter;
    DWORD dwLastSyncTime;
    DWORD MaxReply_msec;
    int ReservTaskErrorCount;
    DWORD OtherControllerStatus;
    DWORD OtherFaultLevel;
    DWORD LastUpdateOtherControllerStatus;
    DWORD LastTryUpdateOtherControllerStatus;
    int64_t LastUpdateOtherControllerStatusTimer;
    DWORD LastMasterFailClearOut;
    DWORD TimeForMasterToTakeControl;
    RDateTime LastSystemInReservChanged;
    // RBOOLEAN8		ErrorConnectionWithUpperLevel;	//Текущий контроллер не опрашивается, а другой контроллер
    // опрашивается DWORD OtherLastErrorConnectionWithUpperLevel;	//Время, когда у другого контроллера была ошибка
    // соединения в последний раз DWORD LastUpdateOtherLastErrorConnectionWithUpperLevel;	//Время проверки
    // OtherLastErrorConnectionWithUpperLevel
    //	int ControllerStatus;
    BYTE PTR pRecvBuffer;
    BYTE PTR pSendBuffer;
    DWORD MaxReservTaskErrorCount;
    DWORD ChangeToSlaveCounter;
    WORD RetryCount;
    RBOOLEAN RedundancyChangeToMain;
    const char* ThisAddresses;

#ifndef _WIN32
    int fd_eth_control;
#endif
} RESERV_TASK_CB;

extern RESERV_TASK_CB ReservTaskCB;
extern RHANDLE ReservTaskRPID;
extern BYTE ReservTaskPresent;
extern int g_maxReservUdpFragmentSize;
extern int g_enableMiniDump;

extern int InitReservTask(void);
extern int ReservTaskFinish(void);
extern RBOOLEAN NeedToSwitchOnFault(RBOOLEAN need_lock);
extern int GetOtherControllerState(DWORD* otherStatus, DWORD* otherFaultLevel, int64_t* otherTimer);

DECLARE_THREAD(ReservTaskThread);

EXTERN_C int SetReservControllerStatus(DWORD otherStatus,
                                       DWORD otherFaultLevel,
                                       int64_t otherTimer,
                                       const char* method);
EXTERN_C void UpdateReservStatusCore();
EXTERN_C DWORD GetOtherControllerStatus();
EXTERN_C void GetReservTaskStat(RESERV_TASK_STAT* stat);

#endif  // #ifndef RESERV_H
