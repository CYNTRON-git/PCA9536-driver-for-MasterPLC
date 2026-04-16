#ifndef DRV_USER_H
#define DRV_USER_H
#include "main.h"
#include "task.h"
#include "fb.h"

BEGIN_EXTERN_C
_PACKED(typedef struct DRV_MODULE_CONFIG {
    WORD ConfigSize;
    WORD FormatVersion;
    WORD Address;
    BYTE ModuleScaleId;
    BYTE Reserv;
    WORD NumFirstParam;
    WORD QuanParams;
    char ModuleName[10];
    BYTE ModulePinType;
    BYTE PinType;
    DWORD Flags;
    // BYTE*	Scale;
    // BYTE*	Property;
})
DRV_MODULE_CONFIG;

_PACKED(typedef struct DRV_MODULE {
    DRV_MODULE_CONFIG PTR pConfig;
    void PTR pTaskCB;
    WORD NumFirstParam;  //Позиция первого параметра модуля в массиве параметров задачи
    WORD QuanParams;     //Количество параметров в модуле
    char* ModuleName;    //Тип модуля (строка 10 символов)
    WORD ModuleType;     //Числовое значения типа (из строки ModuleName выделяется число)
    WORD Address;        //Адресс модуля (задается в скаде для модулей, имеющих адрессацию)
    RBOOLEAN8 OldExecute;
    BYTE ModulePinType;
    BYTE ModuleScaleId;
    BYTE PinType;
    DWORD Flags;
    BYTE* Scale;
    BYTE* Property;
    RBOOLEAN8 OldWrite;
    BYTE Reserv;

    WORD ExtraConfigSize;
    BYTE* ExtraConfig;

    WORD ReadingErrorsInCurCycle;
    WORD ReadingRequestsInCurCycle;
    WORD WritingErrorsInCurCycle;
    WORD ErrorReadingCount;  //Количество последовательных ошибочных циклов чтения данных с
                             //устройства
    WORD ErrorWritingCount;  //Количество последовательных ошибочных циклов записи данных в
                             //устройство
    int ModuleIndex;
})
DRV_MODULE;

enum EDriverFlags {
    dfInitializePort = 1,
    dfInitializeOutParams = 2,
    dfUseLastParamFault = 4,
    dfDisableCycleStatistics = 8,
    dfAlwaysWorkInSlave = 0x10,
    dfExecuteOnSlave = 0x20,
    dfWriteOnSlave = 0x40,
    dfDriverFaultControl = 0x80,
};

enum EDriverProtocolFlags {
    dpfUseChecksum = 0x1,
    dpfWriteExtendedConfig = 0x2,
    dpfWriteProperties = 0x4,
    dpfUseRS232 = 0x8
};

_PACKED(typedef struct SCALE_PARAM {
    DWORD NumParam;
    WORD Rule;
    WORD Flags;
    double SourceLo;
    double SourceHi;
    double DestLo;
    double DestHi;
    double Param;
})
SCALE_PARAM;

_PACKED_BEGIN

typedef struct SERIAL_TASK_CB {
    TASK_HEADER_EX PTR pHeader;
    TASK_PARAM PTR pListParams;
    RPARAM PTR pParams;     //Массив текущих значений параметров
    RPARAM PTR pOutParams;  //Массив предыдущих значений параметров для контроля изменения
    int Port;
    DWORD Baud;    
    int Data;
    int Parity;
    int Stop;
    WORD ProtocolType;  //Идентификатор драйвера
    BYTE ProtocolFlags;
    WORD TaskIndex;
    BYTE* Property;

    WORD ModuleStructSize;
    WORD QuanModules;  //Количество опрашиваемых устройств
    RBOOLEAN8 bWriteFault;
    RBOOLEAN8 bReadFault;
    int GlobalArrayChangeCounter;

    DWORD DriverFlags;
    BYTE PTR pDriverCB;  //Область для хранения служебных данных драйвера
    DWORD SizeRecvBuffer;
    BYTE PTR RecvBuffer;

    DWORD SizeConfig;  //Размер конфигурации драйвера
    BYTE PTR pConfig;  //Конфигурация драйвера (формируется по настройкам в скаде)

    DWORD cicle_counter;
    DWORD LenLastStep_msec;
    DWORD error_counter;
    DWORD single_error_counter;
    DWORD StartLastStep_msec;

    // блоктровки запука для прозрачного драйвера
#ifdef USE_PLC_OPTION_PROXY_COM
    char semName[10];            // имя для семафора
    R_CRITICAL_SECTION csBlock;  //
    RBOOLEAN bInWork;            //
    RBOOLEAN bBlocked;           //
    struct SERIAL_TASK_CB PTR pProxyPort;
#endif

    WORD QuanScaleParams;
    WORD wReserv1;
    struct SCALE_PARAM PTR pScaleParams;
    int CountOutputParams;

    RBOOLEAN8 bTaskFaultOnAllModulesFault;
    RBOOLEAN8 bFaultOnTaskFault;
    int nResetFaultDelayInSlave;
    int nWriteDelayOnStartCycles;
    int nSkipWriteStepsInMaster;

} R_PACKED SERIAL_TASK_CB;

_PACKED_END

extern DRV_MODULE PTR GetSerialModule(SERIAL_TASK_CB PTR pSerialTask, int nModule);

typedef struct PORT_DRIVER_DATA {
    PortDriverFuncGetRequestPriority funcGetRequestPriority;
    PortDriverFuncGetRequest funcGetRequest;
    PortDriverFuncOnReply funcOnReply;
    PortDriverFuncOnRestart funcOnRestart;
    void* pData;
    int nMissedCycles;
} PORT_DRIVER_DATA;

//Описание процедуры
typedef enum DRIVERMODE { dmInit, dmRead, dmWrite, dmWriteByChange, dmDone } DRIVER_MODE;

enum SerialProtocolType {
    sptADAM = 10,
    sptImitation = 11,
    sptOther = 12,

    sptDANFOSS = 13,
    sptMERCURY230 = 14,
    sptLOGIKASPT961 = 15,
    sptLOGIKASPG762 = 16,
    sptLOGIKASPT942 = 17,
    sptPULSAR = 18,
    sptMITSUB = 20,
    sptModbusRTUMaster = 21,
    sptModbusTCPMaster = 22,
    sptModbusUDPMaster = 23,
    sptELPK4 = 24,
    sptTREI_STBUS = 25,  // Протокол ST-BUS TREI
    sptFastwell_FBUS = 26,
    sptRegul = 27,
    sptTREI_OSATEC = 28,
    sptModbusASCIIMaster = 29,
    sptClassDriver = 30,
    sptModbusRTUOverTCPMaster = 31,
    sptSerebrumDriver = 32,
    sptVKT9RTU = 33,
    sptVKT9TCP = 34,
    sptVKT7RTU = 37,
    sptVKT7TCP = 38,
    sptSET4RTU = 35,
    sptSET4TCP = 36,    
	sptMercuryRTU = 39,
	sptMercuryTCP = 40,
	sptEnergoRTU = 41,
    sptEnergoTCP = 42,
    sptPulsarRTU = 43,
    sptPulsarTCP = 44,
    sptVKT5RTU = 45,
    sptVKT5TCP = 46,
    sptEskoRTU = 47,
    sptEskoTCP = 48,
    sptFirstUserDriver = 50, 
    sptVzljotRTU = 100,
    sptVzljotTCP = 101,
    sptMktsRTU=102,
    sptMktsTCP = 103,
    sptAmtRTU=104,
    sptAmtTCP = 105
};

typedef int (*DriverProcType)(DRIVER_MODE mode, SERIAL_TASK_CB PTR taskCB);
extern DriverProcType Drivers_proc_list[];
extern int RegisterDriverFunction(int DriverID, DriverProcType DriverMyProc);

#define DRIVER_USER_TYPE 50  // max type = 99
#define MAX_DRIVER_USER_TYPE 99

//Доступ к параметрам
extern int64_t GetModuleParamID(DRV_MODULE PTR pModule, DWORD param);
extern DWORD GetModuleParamDirection(DRV_MODULE PTR pModule,
    DWORD param);  // Определение направления параметра (вход/выход)
extern RPARAMPtr GetModuleParam(DRV_MODULE PTR pModule, WORD param);
extern int GetFirstParamInGroup(DRV_MODULE PTR pModule, WORD GroupID);
extern int GetParamCountInGroup(DRV_MODULE PTR pModule, WORD GroupID);
extern DWORD CreatePinID(int index, int groupID);
extern RPARAM PTR FindModuleParamByID(DRV_MODULE PTR pModule, DWORD dwParamID);
extern int FindModuleParamIndexByID(DRV_MODULE PTR pModule, DWORD dwParamID);
extern SERIAL_TASK_CB PTR FindSerialTask(int nPort);
extern SERIAL_TASK_CB PTR GetSerialTask(int index);
extern int GetQuanSerialTasks();
extern int GetQuanUserTasks();

//Высокоуровневые функции работы с портом
extern int SendReceiveCOMCommand(int iPort,
    const char* SendBuffer,
    int SendCount,
    char* RecvBuffer,
    int RecvBufferSize,
    RINTEGER iTimeout,
    char cFinishChar);

extern int SendReceiveCOMCommandByteAfterStop(int iPort, const char *SendBuffer, int SendCount, char *RecvBuffer, int RecvBufferSize, RINTEGER iTimeout, char* cFinishChars, int CountFinishChars, int CountAfter);
extern int SendAndReceiveDLMS(int iPort,
                      const char* SendBuffer,
                      int SendCount,
                      char* RecvBuffer,
                      int RecvBufferSize,
                      RINTEGER iTimeout,
                      RINTEGER TimeoutInterSymbol);
extern int SendReceiveADAMCommand(int iPort,
    char* SendBuffer,
    char* RecvBuffer,
    int RecvBufferSize,
    int iChksum,
    RINTEGER iTimeout);
extern void ComGetInfo(char* BufInfo, int Size, int ComNumber);

//Работа с задачей и модулями
extern void SetSerialTaskAlive(SERIAL_TASK_CB PTR pSerialTask);
extern void CanRunSerialTaskWriteStep(SERIAL_TASK_CB PTR pSerialTaskCB);
extern void SetReadingSerialModuleFaultState(DRV_MODULE PTR pModule, RBOOLEAN bFault, const char* error);
extern void SetWritingSerialModuleFaultState(DRV_MODULE PTR pModule, RBOOLEAN bFault, const char* error);
extern void UpdateModulesWriteFailStatus(SERIAL_TASK_CB PTR pSerialTask, RBOOLEAN isByChange);
extern void UpdateModulesReadFailStatus(SERIAL_TASK_CB PTR pSerialTask);
extern void SetProtocolFaultStatus(int task, RBOOLEAN readFault, RBOOLEAN writeFault);

extern void InitInputValuesBeforeReading(RPARAM PTR pParams, WORD ParamCount, RBOOLEAN bInUse);
extern RBOOLEAN IsNeedConnectModule(DRV_MODULE PTR pModule);
extern RBOOLEAN IsNeedReadModule(DRV_MODULE PTR pModule);
extern RBOOLEAN IsNeedWriteModule(DRV_MODULE PTR pModule, RBOOLEAN onlyByChange);
extern RBOOLEAN IsModulesImitation();
extern RINTEGER GetModuleSetChannel(DRV_MODULE PTR pModule);
extern void SetModuleNumChannel(DRV_MODULE PTR pModule, RINTEGER channel);

    //Работа со свойствами
_PACKED(typedef struct PROPERTY_HEADER {
    DWORD dwSize;
    BYTE HeaderSize;
    BYTE byValueType;
    WORD wCategory;
    DWORD dwPropID;
    DWORD dwValuesCount;
})
PROPERTY_HEADER;

enum SysProperties {
    propIDPinIDs = 1,
    propIDChannelIDs = 2,
    propIDFaultIndex = 3,
    propIDWriteIndex = 4,
    propIDExecuteIndex = 5,
    propIDConnectIndex = 6,
    propIDWriteCondition = 7,
    propIDNumChannelIndex = 8,
    propIDSetChannelIndex = 9,

    propIDExecuteOnSlave = 20,
    propIDTaskFaultOnAllModulesFault = 21,
    propIDWriteDelayOnStartCycles = 22,
    propIDFaultOnTaskFault = 23,
	propIDResetFaultDelayInSlave = 24,
	propIDWriteOnSlave = 25
};

enum PropertyCategory {
    propCatDefault = 0,
    propCatDefault2 = 1,
    propCatDefault3 = 2,
    propCatDefault4 = 3,
    propCatDefault5 = 4,
    propCatFirstParamInGroup = 100,
    propCatParamCountInGroup = 101,
    propCatSysProperties = 102
};

/// Режимы способа записи переменных драйвера
enum ProtocolWriteCondition {   pwcByChange = 1,    // по изменению значения выхода
                                pwcByChangeTime,    // по ишменению значения или временной метки выхода 
                                pwcByTime,          // по заданному интервалу задачи
                                pwcByCondition,      // по срабатыванию переменной условия записи драйвера
                                pwcByChangeStatusCode,     //по изменению значения или признака качества выхода
                                pwcByChangeStatusCodeTime  //по изменению значения, признака качества или временной метки выхода
                            };

extern BYTE PTR GetPropertyValues(BYTE PTR pProperty, WORD wCategory, DWORD dwPropID);
extern int GetPropertyValuesCount(BYTE PTR pProperty, WORD wCategory, DWORD dwPropID);
extern RINTEGER GetIntegerProperty(BYTE PTR pProperty,
    WORD wCategory,
    DWORD dwPropID,
    RINTEGER DefaultValue);
extern RFLOAT4B GetFloatProperty(BYTE PTR pProperty,
    WORD wCategory,
    DWORD dwPropID,
    RFLOAT4B DefaultValue);
extern RBOOLEAN8 GetBooleanProperty(BYTE PTR pProperty,
    WORD wCategory,
    DWORD dwPropID,
    RBOOLEAN8 DefaultValue);
extern const char* GetStringProperty(BYTE PTR pProperty,
    WORD wCategory,
    DWORD dwPropID,
    const char* DefaultValue);

extern RINTEGER GetModuleIntegerProperty(DRV_MODULE PTR pModule,
    WORD wCategory,
    DWORD dwPropID,
    RINTEGER DefaultValue);
extern RFLOAT4B GetModuleFloatProperty(DRV_MODULE PTR pModule,
    WORD wCategory,
    DWORD dwPropID,
    RFLOAT4B DefaultValue);
extern RBOOLEAN8 GetModuleBooleanProperty(DRV_MODULE PTR pModule,
    WORD wCategory,
    DWORD dwPropID,
    RBOOLEAN8 DefaultValue);
extern char* GetModuleStringProperty(DRV_MODULE PTR pModule,
    WORD wCategory,
    DWORD dwPropID,
    const char* DefaultValue);

extern RINTEGER GetSerialDrvIntegerProperty(SERIAL_TASK_CB PTR pSerialTaskCB,
    WORD wCategory,
    DWORD dwPropID,
    RINTEGER DefaultValue);
extern RFLOAT4B GetSerialDrvFloatProperty(SERIAL_TASK_CB PTR pSerialTaskCB,
    WORD wCategory,
    DWORD dwPropID,
    RFLOAT4B DefaultValue);
extern RBOOLEAN8 GetSerialDrvBooleanProperty(SERIAL_TASK_CB PTR pSerialTaskCB,
    WORD wCategory,
    DWORD dwPropID,
    RBOOLEAN8 DefaultValue);
const char* GetSerialDrvStringProperty(SERIAL_TASK_CB PTR pSerialTaskCB,
    WORD wCategory,
    DWORD dwPropID,
    const char* DefaultValue);

void WriteBooleanParams(DRV_MODULE* pModule,
    int start,
    int num,
    WORD qcode,
    char* vals,
    int vals_num);
void WriteIntegerParams(DRV_MODULE* pModule,
    int start,
    int num,
    WORD qcode,
    int* vals,
    int vals_num);
void WriteFloatParams(DRV_MODULE* pModule,
    int start,
    int num,
    WORD qcode,
    float* vals,
    int vals_num);

#ifdef WIN32
#    define MPLC_DRIVER_API __declspec(dllexport)
#else
#    define MPLC_DRIVER_API
#endif

END_EXTERN_C

#endif
