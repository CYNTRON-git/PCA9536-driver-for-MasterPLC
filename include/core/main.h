#ifndef MAIN_H
#define MAIN_H

#include "config.h"
#include "includes.h"
// clang-format off
BEGIN_EXTERN_C

//Типы параметров
#define IOTYPE_FLOAT    0
#define IOTYPE_BOOLEAN  1
#define IOTYPE_INTEGER  2
#define IOTYPE_UNKNOWN  3


typedef BYTE RBOOLEAN8;
typedef RBOOLEAN PTR RBOOLEANPtr;
typedef RINTEGER PTR RINTEGERPtr;
typedef RFLOAT PTR RFLOATPtr;

//Варианты значения типа BOOLEAN
#define RTRUE  1
#define RFALSE 0

#ifndef TRUE
	#define TRUE	1
	#define FALSE	0
#endif

//Флаги значения (для поля Flags)
#define VALUE_NOT_DEFINED 1
#define VALUE_BLOCKED 2
#define VALUE_BLOCKED_ONE 4	//Блокировка на одну операцию записи
#define VALUE_PROCESSED 8	//Значение входа отмасштабировано уже
#define VALUE_FORCE 0x10	//Значение необходимо разово передать по межконтроллерной связи независимо от изменения

typedef int64_t RDateTime;
#define RDateTimeZero 0

//Структура параметра
_PACKED(typedef struct RPARAM
{
    WORD Quality:8;
    WORD Flags:6;
    WORD Type:2;
	WORD Reserv;
	int StatusCode;
	RDateTime Time;
	union Value
	{
        RFLOAT    Float;
        RBOOLEAN  Boolean;
        RINTEGER  Integer;
	} Value;
} )  RPARAM;
typedef RPARAM PTR RPARAMPtr;

#define SIZE_RPARAM sizeof(RPARAM)

_PACKED(typedef struct RPARAMOld
{
	WORD Quality : 8;
	WORD Flags : 6;
	WORD Type : 2;
	union ValueRPARAMOld
	{
		RFLOAT    Float;
		RBOOLEAN  Boolean;
		RINTEGER  Integer;
	} Value;
})  RPARAMOld;

// --------------------------------------------------------
// return codes
// --------------------------------------------------------
#if !defined(S_OK)
	#define S_OK 0
#endif

#if !defined(S_FALSE)
#define S_FALSE 1
#endif

#define S_VALUE_NOT_DEFINED 1 // for ReadXXX
#define S_VALUE_IS_BAD 2 // for ReadXXXGood
#define E_FAIL -1 // unknown error
#define E_POINTER -2 // invalid pointer
#define E_INVALID_INDEX -3 // invalid parametr index
#define E_INVALIDARG  -4
#define E_INVALID_PORT_NAME -6
#define E_TYPE -50

#ifndef NoError
	#define NoError	0
#endif

// --------------------------------------------------------
// OPC Quality flags
// Masks for extracting quality subfields
// (note 'status' mask also includes 'Quality' bits)
// --------------------------------------------------------
#define    OPC_QUALITY_MASK            0xC0
#define    OPC_STATUS_MASK             0xFC
#define    OPC_LIMIT_MASK              0x03
// Values for QUALITY_MASK bit field
#define    OPC_QUALITY_BAD             0x00
#define    OPC_QUALITY_UNCERTAIN       0x40
#define    OPC_QUALITY_GOOD            0xC0
// STATUS_MASK Values for Quality = BAD
#define    OPC_QUALITY_CONFIG_ERROR    0x04
#define    OPC_QUALITY_NOT_CONNECTED   0x08
#define    OPC_QUALITY_DEVICE_FAILURE  0x0c
#define    OPC_QUALITY_SENSOR_FAILURE  0x10
#define    OPC_QUALITY_LAST_KNOWN      0x14
#define    OPC_QUALITY_COMM_FAILURE    0x18
#define    OPC_QUALITY_OUT_OF_SERVICE  0x1C
#define    OPC_QUALITY_WAITING_FOR_INITIAL_DATA 0x20
// STATUS_MASK Values for Quality = UNCERTAIN
#define    OPC_QUALITY_LAST_USABLE     0x44
#define    OPC_QUALITY_SENSOR_CAL      0x50
#define    OPC_QUALITY_EGU_EXCEEDED    0x54
#define    OPC_QUALITY_SUB_NORMAL      0x58

// STATUS_MASK Values for Quality = GOOD
#define    OPC_QUALITY_LOCAL_OVERRIDE  0xD8

//Функции работы с параметрами
#define  IsQualityGood(Quality) (((Quality) & OPC_QUALITY_MASK) == OPC_QUALITY_GOOD)
extern RBOOLEAN IsValueDefined(RPARAM PTR param);
extern int iReadBoolean(RPARAM PTR src, RBOOLEAN PTR pBoolean);
extern int iReadInteger(RPARAM PTR src, RINTEGER PTR pInteger);
extern int iReadFloat(RPARAM PTR src, RFLOAT PTR pFloat);
extern int iWriteFloatRaw(RPARAM PTR dst, RFLOAT fvalue);
extern int iWriteIntegerRaw(RPARAM PTR dst, RINTEGER ivalue);
extern int iWriteBooleanRaw(RPARAM PTR dst, RBOOLEAN bvalue);
extern int iWriteFloat(RPARAM PTR dst, RFLOAT fvalue);
extern int iWriteInteger(RPARAM PTR dst, RINTEGER ivalue);
extern int iWriteBoolean(RPARAM PTR dst, RBOOLEAN bvalue);
extern int iWriteFloatGood(RPARAM PTR dst, RFLOAT fvalue);
extern int iWriteIntegerGood(RPARAM PTR dst, RINTEGER ivalue);
extern int iWriteBooleanGood(RPARAM PTR dst, RBOOLEAN bvalue);
extern int iReadBooleanGood(RPARAM PTR src, RBOOLEAN PTR pBoolean);
extern int iReadIntegerGood(RPARAM PTR src, RINTEGER PTR pInteger);
extern int iReadFloatGood(RPARAM PTR src, RFLOAT PTR pFloat);
extern int iWriteInvalidValue(RPARAM PTR dst, WORD Quality);
//time - время FileTime, если 0 - ставится текущее; statusCode - признак каачества OPC UA (0 - Good)
extern int iWriteFloatEx(RPARAM PTR dst, RFLOAT fvalue, RDateTime time, int statusCode);
extern int iWriteIntegerEx(RPARAM PTR dst, RINTEGER ivalue, RDateTime time, int statusCode);
extern int iWriteBooleanEx(RPARAM PTR dst, RBOOLEAN bvalue, RDateTime time, int statusCode);

//Копирование и обнуление параметров
#define cpy_noautocast     0x01
#define cpy_checkdefined   0x02
#define cpy_noflags        0x04
#define cpy_noquality      0x08
#define cpy_notimestamp    0x10
#define cpy_fullrparam     0x20

extern int iCopyParam(RPARAM PTR src, RPARAM PTR dst, WORD flags);
extern RBOOLEAN IsParamsEqual(RPARAM PTR src, RPARAM PTR dst, WORD flags);
extern int iDoZeroParam(RPARAM PTR param);
extern void iClearParams(RPARAM PTR params, int nParamCount);
extern int iWriteParam(RPARAM PTR param, BYTE* stream, WORD flags);
extern int iReadParam(RPARAM PTR param, BYTE* stream, WORD flags);
extern int iWriteQuality(RPARAMPtr pParam, int Quality);
extern int iWriteStatusCode(RPARAMPtr pParam, int statusCode);

// date/time
_PACKED (typedef struct RTIME
{
	WORD  year;
	WORD  mon;
	WORD  day;
	WORD  hour;
	WORD  min;
	WORD  sec;
	WORD  msec;
})  RTIME;

_PACKED(typedef struct TIME_8B
{
	BYTE  year;
	BYTE  mon;
	BYTE  day;
	BYTE  hour;
	BYTE  min;
	BYTE  sec;
	WORD  msec;
})  TIME_8B;

extern int CompareTIME_8B(const TIME_8B PTR time1, const TIME_8B PTR time2);
extern int CompareRTIME(const RTIME PTR time1, const RTIME PTR time2);

extern DWORD RTimeMSec;

// memory allocation functions
extern void PTR GetMem(size_t n);
#if defined(PLC_ST_MEMPOL)
	extern void PTR GetNamedMem(size_t n, const char* name);
	extern void PTR DeleteNamedMem(void* p, const char* name);
#else
	#define GetNamedMem(n, name)  GetMem(n)
	#define DeleteNamedMem(p, name)  FreeMem(p)
#endif
extern void PTR GetZeroMem(size_t n);
extern void FreeMem(void PTR pMem);
extern void ClearMem(void);
extern DWORD GetFreeMemSize(void);
extern int64_t GetProcMemSize(void);
extern void MemCpy(void PTR dest, const void PTR src, size_t n);
extern RBOOLEAN MemEquals(const void PTR dest, const void PTR src, size_t n);
extern RBOOLEAN MemCpyIfChanged(void PTR dest, const void PTR src, size_t n);

// date/time functions
extern int RGetDateTime(RTIME PTR pRTime);
extern int RGetLocalDateTime(RTIME PTR pRTime);
extern int RSetDateTime(RTIME PTR pRTime, DWORD dwFlags);
extern DWORD RGetTime_ms(void );
extern DWORD RGetTime_ms2(void );
extern DWORD DeltaT(DWORD begin_time, DWORD end_time);
extern DWORD SubstractT(DWORD begin_time, int delta);
extern int Time8BToRTime(TIME_8B PTR src, RTIME PTR dest);
extern int RTimeToTime8B(RTIME PTR src, TIME_8B PTR dest);
extern int RDiffDateTime(RTIME PTR pBeginTime, RTIME PTR pEndTime);
extern int RGetDateTimeInternal(RTIME PTR pRTime);
extern void RTimeAddSec(RTIME PTR pRTime, int sec);
extern void RTimeToLocal(RTIME PTR pRTime);
extern RDateTime RDateTimeToLocalTime(RDateTime time);
extern RDateTime RDateTimeToUTCTime(RDateTime time);
extern int64_t GetLocalTimeDiff();
extern FILETIME DATEtoFT(DATE rfloat);
extern DATE FTtoDATE(FILETIME ft);
extern FILETIME ULLtoFT(LONGLONG qw);
extern LONGLONG FTtoULL(FILETIME fileTime);
extern FILETIME getFileTime();

extern FILETIME FTdiffFT(FILETIME ft1, FILETIME ft2);
extern LONGLONG FTdiffMSEC(FILETIME ft1, FILETIME ft2);
extern int RTimeToFileTime(RTIME * pRTime, FILETIME *ft);
extern time_t FileTimeToTimet(int64_t time);
extern void FileTimeToInt64(FILETIME ft, int64_t *time);
extern int64_t getInt64FileTime();
extern int64_t getTimePoint(); // Uptime in FILETIME Ticks
extern int RTimeToInt64(RTIME * pRTime, int64_t *time);
extern FILETIME FTadd(FILETIME ft1, LONGLONG ms);
extern int FileTimeToRTime(FILETIME ft, RTIME* pRTime);
extern int64_t TIMEtoINT64(double t);
extern void TimetToFileTime(time_t t, LPFILETIME pft);
extern RDateTime getRDateTime();
extern RDateTime getLocalRDateTime();
extern int RTimeToVariantTime(RTIME PTR pTime, double* pDateOut);
extern int VariantTimeToRTime(double dateIn, RTIME PTR pTime);

#define RDateTimeToFT ULLtoFT
#define RFTToDateTime FTtoULL

//Helper functions
#define RES int res=0
#define CHECK_RESULT(op) do {int res = op; if ( res != S_OK ) return res;} while(0)
extern void DeleteConfig(void);
extern void ExitProgram(int code);
extern void Terminate(int code);
extern void StopAndExitProgram(int code);
extern void PrintIPAddress(DWORD dwIP);
extern DWORD IPAddressFromString(const char* str);
extern void PrintParamValue(RPARAM PTR pValue);
extern void InitConfigRegistration(void);
extern int CharToHexValue(char c);
extern int ExtractHexValue(char* str);
DWORD ExtractHexValue4B(char* str);
DWORD ExtractHexValue8B(char* str);
void ExtractHexValues(const char* str, BYTE* buf, int count);

extern void RSleep_ms(int32_t sl_ms);
#if !defined(PLC_LINPAC)
extern WORD GetCRC16(BYTE PTR puchMsg, int DataLen);
#endif
extern void CopyInt2HLToBytesArray(int value, char* buffer);
extern void CopyInt2LHToBytesArray(int value, char* buffer);
extern void CopyInt4LHToBytesArray(int value, char* buffer);
extern void CopyInt4HLToBytesArray(int value, char* buffer);

extern void SwapBytes(BYTE* b1, BYTE* b2);
extern void Swap4(BYTE* buffer);
extern void Swap2(BYTE* buffer);
extern RBOOLEAN IsEnableUserTrace();
extern void SetUserTrace(RBOOLEAN val);
typedef enum {
	strictConversion = 0,
	lenientConversion
} ConversionFlags;

typedef enum {
	conversionOK,           /* conversion successful */
	sourceExhausted,        /* partial character in source, but hit end */
	targetExhausted,        /* insuff. room in target for conversion */
	sourceIllegal           /* source sequence is illegal/malformed */
} ConversionResult;

typedef unsigned int    C_UTF32;  /* at least 32 bits */
typedef unsigned short  C_UTF16;  /* at least 16 bits */
typedef unsigned char   C_UTF8;   /* typically 8 bits */

extern int convert_utf8_to_utf16(const char* utf8, size_t n, BYTE* utf16, int maxTextLen, int* inputPos, RBOOLEAN isLE);
extern int convert_utf8_to_windows1251(const char* utf8, char* windows1251, size_t n);
extern ConversionResult convert_utf16_to_utf8(
	const C_UTF16** sourceStart, const C_UTF16* sourceEnd,
	C_UTF8** targetStart, C_UTF8* targetEnd, ConversionFlags flags);
extern ConversionResult ConvertUTF8toUTF16(
	const C_UTF8** sourceStart, const C_UTF8* sourceEnd,
	C_UTF16** targetStart, C_UTF16* targetEnd, ConversionFlags flags);
extern int ConvertUTF8toTCHAR(const char* utf8, TCHAR* szPath, size_t n);

extern int GetExemplarNum(void);
#if defined(USE_FILESYSTEM)
	extern int IOCreatePath(TCHAR* szPath);
	extern int IOMoveFile(const char* szOldFile, const char* szNewFile);
	extern int IOTMoveFile(const TCHAR* szOldFile, const TCHAR* szNewFile);
	extern RBOOLEAN IOTFileExists(const TCHAR* szFile);
	extern  const TCHAR* GetBaseDataPath(void);
    extern  const TCHAR* GetBaseProgramPath(void);
	extern const char* GetWebServerPath(void);
	extern  int GetModulePath(TCHAR* path);
	extern int IODeleteFile(const TCHAR* name);
	void IOGetTFilePath(TCHAR* tsFile, const char* shortName, TCHAR* basePath);
#endif

#ifdef MFC3000
extern void RLogMessage(BYTE code, const char* text, DWORD var);
#else
#define RLogMessage(code, text, var)
#endif


//Global array
extern int GlobalArrayChangeCounter;
//extern void ApplyGlobalArrayChange(void);

_PACKED(typedef struct PARAM_ID
{
	DWORD ID;
	DWORD SubID;
})  PARAM_ID;

typedef PARAM_ID R_HUGE_PTR PARAM_IDPtr;
extern RBOOLEAN CompareParamID(PARAM_ID PTR param1, PARAM_ID PTR param2);
extern DWORD PTR pFbByTasksIDs;
extern DWORD GetFbByTasksID(int index);
extern BYTE PTR pGlobalProps;
extern int WriteGlobalParam(int64_t id, RPARAM PTR value, RBOOLEAN bBlockOne);
extern int ReadGlobalParam(int64_t id, RPARAM PTR value);

extern BYTE SystemInReserv;
extern RBOOLEAN AllowControlFromSCADA;
extern WORD NeedRestart;
extern RBOOLEAN EnableUserTrace;
extern DWORD uProxyStub_Trace_Level;
extern WORD QuanUserTasks;
extern WORD QuanSerialTasks;

extern RBOOLEAN IsSystemInReserv(); //В данный момент система в режиме SLAVE
extern RBOOLEAN IsReservStatus(); //Запуск на резервном компьютере
extern RBOOLEAN ReservEnabled();
extern void TryChangeToSlave(const char* reason);
extern RBOOLEAN IsDisableRemotePorts();
extern RPARAM PTR GetResourceParam(WORD paramidx);
extern RBOOLEAN IsResourceParamOutput(WORD paramidx);
extern RBOOLEAN GetResourceBoolean(WORD paramidx, RBOOLEAN bDefault);
extern DWORD GetChangeToSlaveCounter();
extern DWORD GetPLCStateFlags();
extern int GetPLCState();
extern void RegisterThread(const char* name, RBOOLEAN isWorkingThread);
extern void UnregisterThread();
#ifdef __cplusplus
extern "C" {
#endif
extern void PrintRunningThreads();
#ifdef __cplusplus
}
#endif

#define RC_UNIVERSAL_PROTOCOL			100
#define RC_UNIVERSAL_PROTOCOL2			101
#define ST_SUBSCRIBE_CONTROL			85

typedef struct ControllerConfig
{
	BYTE* pRawConfig;
} ControllerConfig;

typedef struct ControllerConfigProps
{
	DWORD MinCyclePause;
} ControllerConfigProps;

typedef enum STProcessorMode
{
	STProcessorModeInit,
	STProcessorModeBeforeRun,
	STProcessorModeAfterRun,
} STProcessorMode;

enum GlobalPropertyIDs
{
	gpContollerTimeFormat = 1,
	gpContollerTimeTimeZone = 2,
	gpContollerAdditionalFilesHash = 10,
	gpContollerUsedFeatures = 11,
	gpContollerUsedFeatureParams = 12,
	gpAllowOPCUA = 20,
	gpOPCUAPort = 21,
	gpModbusTCPPort = 22,
	gpModbusBytesFormat = 23,
	gpOPCUAUsername = 24,
	gpOPCUAPassword = 25,
	gpAllowIEC104 = 26,
	gpIEC104Port = 27,
	gpIEC104_ASDU = 28,
	gpIEC104_SendPeriodical = 29,

	gpProjectName = 30,
	gpProjectId = 31,
	gpProjectVersion = 32,
	gpProjectChange = 33,
	gpIDEVersion = 34,
	gpControllerName = 35,
	gpRTCommandLine = 36,
	gpRestartStorePlace = 37,
	gpSerialPortNameTemplate = 38,
	gpWDTTimeout = 39,
	gpModbusTCPMaxSessions = 40,
	gpModbusTCPIdleTimeout = 41,
	gpAllowIEC61850 = 42,
	gpIEC61850Port = 43,
	/// @ingroup gp2
	/// Признак того, что переменная SYSTEM_XXXX_VALUE трактуется как отдельные атрибуты t, q, stVal. 
	gpIEC61850OpcUa_DataValue_As_Group = 44,
	/// @ingroup gp2
	/// Признак необходимости подключения сервера IEC 61850 по TLS
	gpIEC61850_UseTLS = 45,
	/// @ingroup gp2
	/// Ограничение длины имени узла дерева модели сервера IEC 61850
	gpIEC61850_Max_DAName_Length = 46,

	gpOPCUAClientAddress = 47,
	gpMaxProcessMemory = 48,

	gpProjectIntegrityCheck = 50,
	gpProjectIntegrityBlockStart = 51,
	gpProjectIntegrityPeriod = 52,
	gpSystemIntegrityCheck = 53,
	gpSystemIntegrityBlockStart = 54,
	gpSystemIntegrityPeriod = 55,
	/// Тип NodeID, используемого в OPC UA сервере. См. OpcUa_IdentifierType  
	gpOPCUANodeIdIdentifierType = 60,
	gpNonExclusiveLevelAlarm = 61,
	gpMaxLogCount = 62,
	gpMaxLogSize = 63,
	gpRedundancyStartPause = 64,
	gpGetRemoteArchiveTimeout = 65,
	/// Признак необходимости выполнения OPC UA сервера при работе контроллера в режиме Slave
	gpOPCUARunInSlave = 66,
	gpOPCUAStructBrowseType = 67,
	gpGCOnEveryCycle = 68,
	gpRemoteArchiveLimitPerItem = 69,
	gpModbusTCPRunInSlave = 70,
	gpIEC104_IsUTCTime = 71,
	gpIEC104_InterrogationSendTime = 72,
	gpOPCUAUseTranslitedNames = 73,
    gpMinCyclePause = 74,
	gpRedundancyChangeToMain = 75,
	gpRedundancyFullScanPeriod = 76,
	gpRedundancyNotSwitchOnNetworkFailure = 77,
	gpWDTStopTimeout = 78,
	gpSaveStateMode = 79
};

typedef enum ProgramStartError
{
	pseGood = 0,
	pseExceedInstanceLimit,
	pseNoConfig,
	pseConfigError,
	pseReadConfigError,
	pseUnsupportedVersion,
	pseUnsupportedPlatform,
	pseInitUserTaskError,
	pseInitSerialTaskError,
    pseSystemIntegrityError,
    pseProjectIntegrityError,
	pseSystemIntegrityGood,
    pseProjectIntegrityGood,
    pseMSSQLUnsupportedDBAdapter,
    psePostgresUnsupportedDBAdapter,
    pseMySQLUnsupportedDBAdapter,
	pseUnavailableFeautire,
} ProgramStartError;

enum ProgramExitCode
{
	pecAlarmExit=2,
	pecMemoryError=3,
	pecCodeInterpreter_1=4,
	pecCodeInterpreter_2=5,
	pecTaskFreeze=6,
	pecMemoryExceeded=7,
	//
	pecStop=0,
	//
	pecConfigError=-2,
	pecNetworkError=-5,
	pecProtectKeyNotFound=-6,
	pecUnsupportedProtocolUsed=-7,
	pecErrorOpeningMainUDPSocket=-8,
	pecOutOfMemory=-9,
	pecThreadCreationError=-10,
	//
	//c -101 по -210 ошибки guardant
	pecWrongLinksOptionValuesIndex=-211,
	pecStudyExit=-212
};

enum EEventTypeGroup {
	etgAuditEvent = 1,
	etgSystemAuditEvent = 2,
	etgSystemEvent = 2,
};

enum ESystemAuditEvents
{
	seKeyErrorEvent = 1,
	seProjectIntegrityErrorEvent = 2,
	seSystemIntegrityErrorEvent = 3,
	seSystemUserAction = 4,
	seProjectIntegrityEvent = 5,
	seSystemIntegrityEvent = 6,
	seDatabaseConnectError = 7,
	seDatabaseWriteError = 8,
};

typedef struct {
	DWORD allocSize;
	DWORD size;
	char *s;
} ST_STRING;

extern char* STLoadString(char* pPos, ST_STRING* s);
extern void CopySTString(ST_STRING* s, const char* str, DWORD size);
#define InitSTString(ps) memset(ps, 0, sizeof(ST_STRING))
extern const char* GetSafeSTString(ST_STRING* s);

extern const char* GetConfigDataValue(const char* key);
extern void PrintCurDT(LPCSTR szOperation);
extern RINTEGER GetGlobalIntegerProperty(WORD wCategory, DWORD dwPropID, RINTEGER DefaultValue);
extern RFLOAT4B GetGlobalFloatProperty(WORD wCategory, DWORD dwPropID, RFLOAT4B DefaultValue);
extern RBOOLEAN8 GetGlobalBooleanProperty(WORD wCategory, DWORD dwPropID, RBOOLEAN8 DefaultValue);
extern const char* GetGlobalStringProperty(WORD wCategory, DWORD dwPropID, const char* DefaultValue);

extern const char* GetProgramConfigOption(const char* name);
extern int GetProgramConfigOptionInt(const char* name, int defValue);
extern void SetRestartPLC(void);
extern RBOOLEAN GetRestartPLC(void);
extern void SetPLCHasNewErrors(RBOOLEAN value);
extern unsigned int GetUAQualityByDA(short quality);
extern short GetDAQualityByUA(unsigned int input);

EXTERN_C int String_DT_2_RTIME_61131(const char* str, size_t len, RTIME* rtime, int* out_nsec);
EXTERN_C int String_TIME_2_RTIME_61131(const char* str, struct RTIME* ft, int* out_nsec);
EXTERN_C int String_DATE_2_RTIME_61131(const char* str, struct RTIME* ft);

typedef struct DYN_MEM
{
	R_CRITICAL_SECTION sec;
	BYTE* pos;
	BYTE* buf;
	DWORD size;
	DWORD capacity;
} DYN_MEM;

extern void InitDynMem(DYN_MEM* mem, RBOOLEAN initSec);
extern void CheckDynMemSize(DYN_MEM* mem, int requiredSize);
extern void DynMemSetPos(DYN_MEM* mem, int p);
extern int DynMemGetPos(DYN_MEM* mem);
extern void CopyToDynMem(DYN_MEM* mem, void* src, int size);
extern RBOOLEAN CopyFromDynMem(DYN_MEM* mem, void* dst, int size);
extern void DynMemAttachBuf(DYN_MEM* mem, void* src_buf, int size);

//Critical sections
#if defined(_WIN32) && !defined(PC_WIN32_FB_DEVEL) || defined(LINUX) || defined(ECOS)
	extern void RLockCriticalSection(R_CRITICAL_SECTION* sec);
	extern void RUnlockCriticalSection(R_CRITICAL_SECTION* sec);
	extern void RInitCriticalSection(R_CRITICAL_SECTION* sec, LPCSTR name);
	extern void RDoneCriticalSection(R_CRITICAL_SECTION* sec, LPCSTR name);
#else
    #define RLockCriticalSection(sec)
    #define RUnlockCriticalSection(sec)
	#define RInitCriticalSection(sec, name)
	#define RDoneCriticalSection(sec)
#endif

extern R_CRITICAL_SECTION csGlobalArray;
extern R_CRITICAL_SECTION csFB;
#define LOCK_GLOBAL_ARRAY()  RLockCriticalSection(&csGlobalArray)
#define UNLOCK_GLOBAL_ARRAY()  RUnlockCriticalSection(&csGlobalArray)
#define LOCK_FB_EXECUTION()  RLockCriticalSection(&csFB)
#define UNLOCK_FB_EXECUTION()  RUnlockCriticalSection(&csFB)

#define LOCK_FPU()
#define UNLOCK_FPU()

#ifdef PLC_CONV_NUMBERS
	extern void uncheck_RPARAM(RPARAM PTR  P,int Number);
	extern void check_RPARAM_ONE(RPARAM PTR  P);
#endif

//Для отладки ФБ при возникновении исключений в контроллере
_PACKED(typedef struct TestStruct
{
    RFLOAT f1;
    RFLOAT f2;
    RFLOAT f3;
    RFLOAT f4;
    RFLOAT f5;
    RFLOAT f6;
    RINTEGER i1;
    RINTEGER i2;
    RINTEGER i3;
    RINTEGER i4;
    RINTEGER udp;
    char udp_txt[255];
    const char* fcgi_txt;
})  TestStruct;
extern TestStruct TS;

extern void SetFastCGIText(const char* text);

extern void GetCurrentReserv(DWORD* ip, int* port );

struct CpuLoadState;

extern void DeleteCpuLoadState(struct CpuLoadState*);

extern struct CpuLoadState* InitGetCPULoad();

extern double GetCPULoad(struct CpuLoadState*);

struct ProcCpuLoadState;

extern void DeleteProcCpuLoadState(struct ProcCpuLoadState*);

extern struct ProcCpuLoadState* InitGetProcCPULoad();

extern double GetProcCPULoad(struct ProcCpuLoadState*);

extern long long GetProcThreadCount();

extern long long GetProcDescrCount();

extern const char* G_MPLC_VERSION_HASH;
extern const char* G_MPLC_VERSION_PATH; 
extern const char* G_MPLC_VERSION_BUILD;
extern const char* GetMplcVersonStr();
extern const char* GetMplcVersonHash();
extern const char* GetMplcVersonPath();
extern const char* GetMplcVersonBuid();
extern const char* GetOEMName();
extern const char* GetOEMFullName();
extern int GetMplcVersonMajor();
extern int GetMplcVersonMinor();
extern int GetMplcVersonRevision();

extern const char* GetBindIP();
extern uint16_t GetFastCGIPort();
extern const char* GetFileUploadDir();

extern void SetProgramStartError(ProgramStartError code, const char* errorText);

extern int MakeResponseToMaster(const uint8_t* pRecvBuff, size_t RecvSize, uint8_t* pSendBuff, size_t MaxSendSize, RBOOLEAN isVariableResponseLen, const char* send_addr_str);

extern void FireSystemEvent(int eventTypeGroup, int eventTypeId, const char* message, int priority, int64_t itemId, const char* path);


extern int ProjectIntegrityCheck(const char** err);
extern int SystemIntegrityCheck(const char** err);
extern int FullIntegrityCheck();
extern int GetSystemInfoKey(BYTE* buf, int maxSize);
void SetThreadName(const char* threadName);
extern void PrintStacktrace();
extern size_t GetCountByteUtf8(const char* str, int32_t len, int32_t utf8_pos);
extern int32_t GetUtf8Len(const char* s, size_t len);

#if defined(LINUX)
#define UInt32x32To64(a, b) ((uint64_t)(((uint64_t)((uint32_t)(a))) * ((uint32_t)(b))))
#endif

#ifdef USE_OPCUA
	extern int OpcUa_ProxyStub_Init();
	extern void OpcUa_Clean(void);
#endif

END_EXTERN_C

#endif // #ifndef MAIN_H
