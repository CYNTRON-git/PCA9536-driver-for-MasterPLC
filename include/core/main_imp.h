#ifndef MAIN_IMP_H
#define MAIN_IMP_H

#include "main.h"
#include "fb.h"
#include "wdt_task.h"
#include "zlib.h"

#if !defined(WINPAC_NAME)
#    if defined(PLC_WINPAC)
#        define WINPAC_NAME "winpac_8000"
#    elif defined(PLC_WINPAC_9000)
#        define WINPAC_NAME "winpac_9000"
#    endif
#endif  // WINPAC_NAME
        // clang-format off
//#if !defined(MPLC_CONFIG_NAME)
//	#ifdef _DEBUG
//		#define MPLC_CONFIG_NAME "Debug"
//	#else
//		#define MPLC_CONFIG_NAME  "Release"
//	#endif
//#endif // MPLC_CONFIG_NAME

//
//#if !defined(MPLC_LIBPATH_PREFIX)
//	#if defined(PLC_XPAC8000) && _MSC_VER < 1600
//		#define MPLC_LIBPATH_PREFIX "../../" MPLC_CONFIG_NAME "/"
//		#define MPLC_LIBPATH_TARGET MPLC_CONFIG_NAME 
//	#else 
//		#if defined(WINPAC_NAME)
//			#define MPLC_LIBPATH_TARGET "targets/" WINPAC_NAME "/" MPLC_CONFIG_NAME "/"
//			#define MPLC_LIBPATH_PREFIX "../" MPLC_LIBPATH_TARGET
//		#else 
//			#define MPLC_LIBPATH_PREFIX "" 
//		#endif // WINPAC_NAME
//	#endif 
//#endif // MPLC_LIBPATH_PREFIX


BEGIN_EXTERN_C

// Task types
#define USER_TASK      0
#define SERIAL_TASK    1
#define MODBUS_TASK    2
#define LOCAL_IO_TASK  3
#define RESERV_TASK    5
#define MODEM_UPLEVEL_TASK    6
#define RESOURCE_TASK    7
#define HOT_RESTART_TASK    8
#define ARCHIVE_TASK    9
#define OTHER_CONTROLLER_TASK    10
#define EXTACCESS_TASK    11


_PACKED(typedef struct RESERV_STATE
{
	WORD	wSizeOfHeader;
	BYTE	bFlags;
	BYTE	bVersion;
	RTIME	RTime;
	DWORD	SizeMainBlock;
	DWORD	SizeDefinitionsBlock;
})  RESERV_STATE;

#define CURRENT_RETAIN_VERSION 4  //1 - blocked values, 2 - blocked values in objects, 3 - UTF8 in lua string, 4 MemBlocked

#define	ALIGN_UP(size)	(((DWORD)(size) + 3) & ~3)

//DefinitionsBlock
//DWORD ParametersCount
//PARAM_ID[ParametersCount]
//DWORD FBConfigCount
//{
//	DWORD FBID
//	DWORD Offset
//}

_PACKED(typedef struct ID_OFFSET
{
	DWORD ID;
	DWORD OffsetInReservArray;
	DWORD Size;
})  ID_OFFSET;
typedef ID_OFFSET R_HUGE_PTR ID_OFFSETPtr;

extern int ApplyIDDefinitionsToThereservArray(void);
extern int LoadParamsFromDifferentReservArray(BYTE PTR LoadedReservArray, int size);
extern void RInitDateTime(void);

// Global variables
extern BYTE R_HUGE_PTR pConfiguration;

extern RBOOLEAN CopyGlobalArrayToReservArray(RBOOLEAN bLockMem);

//Reserv array
_PACKED(typedef struct RESERV_ARRAY
{
	DYN_MEM mem;
	DYN_MEM LoadedReservArray;

	DWORD BaseSizeOfReservArray;
	RBOOLEAN UseReservArray;
	RBOOLEAN EnableLoadReservArray;
	RBOOLEAN EnableStoreReservArray;
	WORD ReserveArrayVersion;
	WORD ReserveArrayChangesCounter;
	DWORD QuanBooleanParams;
	DWORD QuanIntegerParams;
	DWORD QuanFloatParams;
	DWORD CountReservFbConfigs;
	DWORD ReserveArrayWriteVersion;
	const char* RedundantNetworkAddresses;
}) RESERV_ARRAY;


extern char g_BindIP[20];  // IP интерфеса для биндинга
extern RESERV_ARRAY ReservArray;

extern WORD QuanTasks;
extern int WorkingThreadsCount;
extern int PLCState;
extern BYTE DisableRemotePorts;
extern RBOOLEAN PLCHasNewErrors;

extern DWORD DemoTimeLeft;

extern int g_MyUDPPort;
extern int ethAddress;
extern int g_projectSessionId;
extern BYTE   mbAddress;
extern ProgramStartError g_ProgramStartError;
extern const char* g_ProgramStartErrorText;
extern const char* g_ProgramStartErrorTexts[];

# define PROJECTID_LEN 8


_PACKED(typedef struct CONTROLLER_SETUP
{
	DWORD	ConfigSize;
	union
	{
		DWORD	FullVersion;
		struct {
			WORD Modifier;
			WORD Main;
		}		Version;
	} ConfigVersion;
	BYTE VersionId[8];
	BYTE ProjectId[PROJECTID_LEN];
	DWORD	ControllerType;
	WORD	ControllerFlags;
	WORD	Reserv1;
	DWORD	Reserv2;
	DWORD	ControllerID;
})  CONTROLLER_SETUP;


//#define GetControllerSetup()	((CONTROLLER_SETUP PTR)(pConfiguration))
extern CONTROLLER_SETUP PTR GetControllerSetup(void);

extern ControllerConfig PTR g_ControllerConfig;
extern ControllerConfigProps g_ControllerConfigProps;

enum EControllerFlags
{
	cfUse8bParam = 0x1,
	cfHotRestartDiffVersion = 0x2,
	cfLoadValuesConfig = 0x4,
	cfImitationMode = 0x8,
	cfUse24bParam = 0x10,
	cfUseEncryption = 0x20
};

enum ESetTimeFlags
{
	stfSetInReserv = 0x1,
	stfUseCorrectionTime = 0x2,
	stfReturnCorrectionTime = 0x4,
	stfOnlyCheckTime = 0x8,
	stfLocalTime = 0x10
};



_PACKED(typedef struct CONFIG_EXTRA_BLOCK
{
	DWORD BlockSize;
	WORD BlockID;
	WORD BlockVersion;
	BYTE Reserv[16];
})  CONFIG_EXTRA_BLOCK;
typedef CONFIG_EXTRA_BLOCK R_HUGE_PTR CONFIG_EXTRA_BLOCKPtr;



#define MAX_EXTRA_CONFIG_ID 10
typedef int (*ExtraBlockParse_proc)(CONFIG_EXTRA_BLOCKPtr pBlock);
void RegisterExtraBlockParseProc(int index, ExtraBlockParse_proc pProc);


enum EExtraBlockID
{
	ExtraBlockID_ParamsID = 1,
	ExtraBlockID_FbByTasksIDs = 2,
	ExtraBlockID_LuaLib = 3,
	ExtraBlockID_Props = 4,
	ExtraBlockID_Data = 5
};

typedef int (*FireSystemEvent_Func)(int eventTypeGroup, int eventTypeId, const char* message, int priority, int64_t itemId, const char* path);
extern void RegisterFireSystemEventFunc(FireSystemEvent_Func pProc);

extern int MplcAesEncrypt(const char* str, int in_size, char** out, int* size);
extern int MplcAesDecrypt(const uint8_t* ctext, int ctext_size, uint8_t* out);
extern int aes_decrypt(const uint8_t* key, const uint8_t* iv, const uint8_t* ctext, int ctext_size, uint8_t* out);
extern int aes_decrypt2(const uint8_t* key, const uint8_t* iv, const uint8_t* ctext, int ctext_size, uint8_t* out, int* out_size);

extern int uncompress_gzip(Bytef* dest, uLong* destLen, const Bytef* source, int* sourceLen);

//FilePath utils
extern int ValidatePath(const char* base, char* path);

extern WORD UseWDT;
extern WORD NeedRestartHost;

#if defined(ENABLE_WATCHDOG) && defined(PC_ADAM)
	extern WORD NeedRefreshWDT;
	#define WATCHDOG_ON()	\
		do { if (UseWDT) WDT_enable(); NeedRefreshWDT = RTRUE; } while (0)
	#define WATCHDOG_OFF()	\
		do { if (UseWDT) WDT_disable(); NeedRefreshWDT = RFALSE; } while (0)
	#define WATCHDOG_REFRESH()	\
		do { if (UseWDT && NeedRefreshWDT) WDT_clear(); } while (0)

#elif defined(ENABLE_WATCHDOG) && (defined(PLC_LINPAC))
	extern WORD NeedRefreshWDT;
	#define WATCHDOG_ON()	\
		{if (UseWDT) PLC_EnableWDT(); NeedRefreshWDT = RTRUE;}
	#define WATCHDOG_OFF()	\
		{if (UseWDT) PLC_DisableWDT(); NeedRefreshWDT = RFALSE;}
	#define WATCHDOG_REFRESH()	\
		{if (NeedRefreshWDT && UseWDT) PLC_RefreshWDT();}

#elif defined(ENABLE_WATCHDOG) && (defined(PLC_DRV_FUNC) || defined(IPC_ANY))
//TODO Check TECONP06 and UC7110
//(defined(PC_MFC) || defined(PC_LOM) || defined(IPC_ANY) || defined(LINUX_MFC) || defined(TKM410) || defined(PLC_WINPAC) || defined(PLCOWEN) || defined(UC7408LX))
extern WORD NeedRefreshWDT;
#define WATCHDOG_ON()	\
		{if (UseWDT) EnableWDT(); NeedRefreshWDT = RTRUE;}
#define WATCHDOG_OFF()	\
		{if (UseWDT) DisableWDT(); NeedRefreshWDT = RFALSE;}
#define WATCHDOG_REFRESH()	\
		{if (NeedRefreshWDT && UseWDT) RefreshWDT();} 
#else
    #define WATCHDOG_ON()
    #define WATCHDOG_OFF()
	#define WATCHDOG_REFRESH()
#endif

#if defined(MTCORE_PROC)
	#define THREAD_RETURN return
#elif defined(_WIN32)
	#define THREAD_RETURN return 0
#elif defined(LINUX)
	#define THREAD_RETURN return 0
#elif defined(ECOS)
	#define THREAD_RETURN return
#endif

#if defined(COOP_MTASK)
	#define END_THREAD	for(;;) {RESCHEDULE();} THREAD_RETURN
#else
	#define END_THREAD	THREAD_RETURN
#endif

#if defined(LINUX_MFC)
	#define SESSION_BIN_DEF_LOC	"/var/sram/"
#elif defined(PC_MFC) || defined(PC_LOM)
	#define SESSION_BIN_DEF_LOC	"D:\\"
#endif

extern TCHAR pathConfigNew[MAX_FILENAME_LEN];
extern TCHAR pathConfigBak[MAX_FILENAME_LEN];
extern TCHAR pathConfigBin[MAX_FILENAME_LEN];
extern TCHAR pathValuesBin[MAX_FILENAME_LEN];
extern TCHAR pathSessionBin[MAX_FILENAME_LEN];
extern TCHAR pathArcivesBin[MAX_FILENAME_LEN];
extern TCHAR BaseDataPath[MAX_FILENAME_LEN];
extern TCHAR BaseProgramPath[MAX_FILENAME_LEN];
extern char WebServerPath[MAX_FILENAME_LEN];

#define CONFIG_BIN  pathConfigBin
#define CONFIG_BAK  pathConfigBak
#define CONFIG_NEW  pathConfigNew
#define PATH_VALUES_BIN  pathValuesBin
#define PATH_ARCHIVES_BIN  pathArcivesBin
extern char g_szServerPassword[];

#if defined(MTCORE_PROC)
	#define DECLARE_THREAD(name) extern void name(PID *pid, WORD wParam)
	#define IMPL_THREAD(name) void name(PID *pid, WORD wParam)
	#define EXIT_THREAD(code) return
#elif defined(_WIN32) || defined(LINUX) || defined(ECOS)
	#define DECLARE_THREAD(name) extern DWORD name(void* lpThreadParameter)
	#define IMPL_THREAD(name) DWORD name(void* lpThreadParameter)
	#define EXIT_THREAD(code) return code
#endif

extern int ProcessUniversalRequest(int nRequestType, const void* pRequest, int nRequestSize, void* pResponse, int nMaxResponseSize, int* pnResponseSize);
extern void GetRTVersion(char* ver, char* verDate, char* serialId, char* platformName, int* platformId, int* usbProtect);
// time_left в миллисекундах
extern void GetProtectInfo(int* key_error, int64_t* time_left);


typedef struct FeatureParameterDef
{
	int id;
	const char* name;
} FeatureParameterDef;

extern FeatureParameterDef FeatureParDefs[];
extern const int FeatureParDefsCount;
typedef enum FeatureParameter
{
	fpSnmpTrapSender=-2,//Виртуальная опция 
	fpSessionsLimit = 1,
	fpPLCConnectionsLimit = 2,	
	fpLicNumber = 3,
	fpInstancesLimit = 4,
	fpInstancesStart = 5,
	fpAllowedVersionDate=6,
	fpAllowPlatformID=7,
	fpBasePlatformType=8,

	fpMercury230 = 50,
	fpDriverSNMP = 51,
	fpDriver61850 = 52,
	fpDriverFBUS = 53,
	fpDriverMQTT = 54,
	fpDriverIEC104 = 55,
	fpDriverSiemensPLC = 56,
	fpDriverOmron = 57,
	fpDriverMitsubishi = 58,
	fpDriverBACnet=59,
	fpDriverCANOPEN=60,
	fpDriverEthernetIP=61,
    fpDriverHelvarNet=62,
	fpMercury230H = 129,// Всё что с произвольным значением - ниже, табличным - выше.
	fpTemplate_Vent = 130,
	//131 - номер для часов работы ключа. Чтобы лишний раз не палиться, тут не указываю. Любой следующий пункт начинать с 132
	fpDriverSET4 = 132,
	fpDriverEnergomera = 133,
	fpDriverVKT7_9 = 134,
	fpDriverPulsar = 135,
	//136 - номер для идентификации работы с SP ключем. Чтобы лишний раз не палиться, тут не указываю. Любой следующий пункт начинать с 137
	fpDriverMKTS=137,
	fpDriverDLMS=138,
	fpDriverTEM=139,
	fpDriverVzljot=140,
	fpDriverEsko=141,
	//Более не используется. Оставлено только для старых версий
	fpDriverLogicaRSB=142,
	fpDriverLogicaSPB=143,
    //BMS опции тут т.к. в числовых больше места
	BMS_VERSION=144,//0 - нет
	BMS_FOLDERS= 145,
	BMS_OBJECTS=146,
	BMS_UNITS=147,
	BMS_OPTION_SCHEMA=148,
	BMS_OPTION_VENT=149,
	BMS_OPTION_HEAT=150,
	BMS_OPTION_WATER=151,
	BMS_OPTION_RECORD=152,
	BMS_OPTION_COND=153,
	BMS_OPTION_FREEZE=154,
	BMS_OPTION_LIGHT=155,
	BMS_OPTION_POWER=156,
	BMS_OPTION_BIM=157,
	fpElectricityGroup=158,
	fpWarmAndEnergyGroup=159,
	fpDriverTBN=160,
	fpDriverRasko=161,
	fpDriverRTSP=162,
	M_M_ITK_S=163,//Под заказ ITK MSRT4D-M&M-ITK-S
	fpMQTTEvents=164,//Булевая, но идет как число, в таких случаях ставить 65000
	fpDriverElemer=165,
	fpContinuous_Operation_Time=166,//Время непрерывной работы (часы)
	fpWork_Till_Date=167,//Время работы до даты
	fpLast
} FeatureParameter;
typedef enum FeatureOptions
{
	foPLC_SESSIONS_ONLY_LOCAL = 0x10,
	foPLC_OPTION_RESERV = 0x20,
	foPLC_OPTION_DB = 0x40,
	foPLC_OPTION_REPORTS = 0x80,
	foPLC_OPTION_INTEGRATION= 0x100,
	foPLC_OPTION_CSHARP = 0x200,
	foPLC_OPTION_GIS = 0x400,
	foPLC_OPTION_TRN = 0x800,
	foPLC_OPTION_CATALOG = 0x1000,
	foPLC_OPTION_SECURITY = 0x2000,
	foPLC_OPTION_OTHER_CONTROLLER = 0x4000,
	foPLC_OPTION_IEC61850_SERVER = 0x8000,
	foPLC_OPTION_FB_JSON = 0x10000,
	foPLC_OPTION_FB_ARCHIVE = 0x20000,
	foPLC_OPTION_FB_COMPORT = 0x40000,
	foPLC_OPTION_FB_FILES = 0x80000,
	foPLC_OPTION_FB_PROCESS = 0x100000,
	foPLC_OPTION_FB_SEND = 0x200000,
	foPLC_OPTION_DB_AVADS_SA = 0x400000,
	foPLC_OPTION_DIGITAL_TWIN = 0x800000,
	foPLC_OPTION_CLICKHOUSE = 0x1000000,
	foPLC_OPTION_BLOCK_WORK_ON_VIRT = 0x2000000//Если выключено, то работаем | MS_STUDY
} FeatureOptions;

typedef enum BMSVesrion
{
	Not_Bms=0,
	Lite=1,
	Standard=2,
	Pro=3,
	Enterprise=4,
    Free=5,

} BMSVesrion;
typedef enum  MSRT4DTVersion
{
	MSOld=0,
	MS4Lite = 1,
	MS4Standard = 2,
	MS4Pro = 3,
	MS4Enterprise = 4,
	MS4Free=5

}MSRT4DVersion;
extern BOOL UsePLC_OPTION_OTHER_CONTROLLER;

//Опции, которые не надо сравнивать при старте конфигурации (проверяются в модулях)
#define PLC_OPTIONS_NOT_CHECK (foPLC_OPTION_REPORTS | foPLC_SESSIONS_ONLY_LOCAL | foPLC_OPTION_TRN)


extern RBOOLEAN CheckTimeRestriction();
extern DWORD GetFeatureParameter(FeatureParameter fp);
extern BOOL IsAvailableFeature(FeatureOptions fo);
extern DWORD GetFeatureParameterEx(FeatureParameter fp, int used_param);
extern int GetFeaturesJSONData(int maxSize, BYTE* buf, RBOOLEAN bDisplay);
extern DWORD GetAvailableFeatures(void);

extern int mainEntry(int argc, char *argv[]);

extern pFB_proc FB_proc_list[INSAT_MAX_FB_TYPE];

extern int ProcessSTSubscribeRequest(BYTE PTR pRecvBuff, int RecvSize, BYTE PTR pSendBuff, int MaxSendSize);

#if defined(ENABLE_MINIDUMP)
extern void CreateMiniDump();
extern void DeleteOldMiniDumps();
#else
//#define CreateMiniDump() (0)
//#define DeleteOldMiniDumps() (0)
#endif
extern LPCSTR AddinCurRequest;
extern DWORD MBRTU_counter;
extern RBOOLEAN ModbusRTUTaskPresent;


// Функция для выполнения задач инициализации для конкретного драйвера
#ifdef PLC_DRV_FUNC
	extern int (*DrvSpecificStartupTask)(void *);
#endif

#ifdef PLC_DRV_USE_TASK_FUNC
	extern void	OnBeforeUserTask(int task);
	extern void	OnAfterUserTask(int task);
#else
	#define	OnBeforeUserTask(task)
	#define	OnAfterUserTask(task)
#endif

extern unsigned char * CharToHexStr(unsigned char *dst, unsigned char *data, int len);
extern void SetProgramOptions(int argc, char** argv);

enum KeyProtectPlatforms
{
    Old = 0,
    Windows_x86 = PLATFORM_WIN32 ,
	Windows_x64=PLATFORM_WIN64,
	Windows_WinCE=PLATFORM_WINCE,
	Windows_WINPAC8000=PLATFORM_WINPAC8000,
	Windows_WINPAC9000=PLATFORM_WINPAC9000,
	Windows_XPAC8000=PLATFORM_XPAC8000,

    Linux_x86 = PLATFORM_LINUX_X86,
	Linux_x64=PLATFORM_LINUX_X64,
	Linux_ADAM3600=PLATFORM_ADAM3600,
	Linux_BOLID=PLATFORM_BOLID,
	Linux_BTUNE=PLATFORM_BTUNE,
	Linux_FASTWELL_MK150=PLATFORM_FASTWELL_MK150,
	Linux_PLC110M2=PLATFORM_PLC110M2,
	Linux_RASPBERRY=PLATFORM_RASPBERRY,
	Linux_TION_28=PLATFORM_TION_28,
	Linux_WAGO750=PLATFORM_WAGO750,
	Linux_WIRENBOARD5 =PLATFORM_WIRENBOARD5,
	Linux_WIRENBOARD6=PLATFORM_WIRENBOARD6,
	Linux_TREI_915_LINUX=PLATFORM_TREI_915_LINUX,
	Linux_MOXA=PLATFORM_MOXA,
	Linux_SEREBRUM_IRIS=PLATFORM_SEREBRUM_IRIS,
	Linux_OPTILOGIC=PLATFORM_OPTILOGIC,
	Linux_PLC210=PLATFORM_PLC210,
	Linux_TION_PRO=PLATFORM_TION_PRO,
	Linux_OSATEC=PLATFORM_OSATEC,
	Linux_OSATEC_X64=PLATFORM_OSATEC_X64,
	Linux_ELPK=PLATFORM_ELPK_X64,
	Linux_ERIS=PLATFORM_ERIS,
	Linux_BAIKALM=PLATFORM_BAIKALM,
	Linux_FIMATIC_C=PLATFORM_FIMATIC_C,
	Linux_BOLID_M3000T_V2=PLATFORM_BOLID_M3000T_V2,
	Linux_BAIKAL_T=PLATFORM_BAIKAL_T,
	Linux_ABAK_K2=PLATFORM_ABAK_K2,
	Linux_ABAK_K3=PLATFORM_ABAK_K3,
	Linux_TREI_500=PLATFORM_TREI_500,
	Linux_KVANTOR=PLATFORM_KVANTOR,
	Linux_WIRENBOARD7=PLATFORM_WIRENBOARD7,
	Linux_ONI_PLC_W=PLATFORM_ONI_PLC_W,
	Linux_BAGET=PLATFORM_BAGET,
	LINUX_TITAN=PLATFORM_TITAN,
	Linux_ARMv7hf=PLATFORM_LINUX_ARMV7HF,
	Linux_ARMv8=PLATFORM_LINUX_ARMV8,

	Linux_TITAN2000=PLATFORM_TITAN2000,
	Linux_NLCSONRSB=PLATFORM_NLSCONRSB,
	Linux_NLSCONA40=PLATFORM_NLSCONA40,
	Linux_UZOLA=PLATFORM_UZOLA,
	Linux_AVRORA=PLATFORM_AVRORA,
	Linux_PLC210RK=PLATFORM_PLC210RK,

	Linux_BAGETPLC1=PLATFORM_BAGET,
	Linux_PLC_INBRES=PLATFORM_PLC_INBRES,

    QNX = PLATFORM_QNX,
	QNX_REGUL=PLATFORM_REGUL,
	QNX_TREI_915=PLATFORM_TREI_915,
	QNX_TREI_903=PLATFORM_TREI_903,

    Elbrus = PLATFORM_ELBRUS,
	Elbrus_X64=PLATFORM_ELBRUS_X64
};

END_EXTERN_C

#endif //MAIN_IMP_H

