
#ifndef DRV_H
#define DRV_H

#include "task.h"
#include "drivers/drv_user.h"

//#if defined(LINUX)
//    #include "mfc_lnx/mfcio.h"
//#endif

#ifdef FALCOM
  #pragma pack(1)
#endif

#define MAX_ERRORS_TO_FAULT	3

enum ModulePinType
{
	mptUnknown = 0,
	mptDiscrete = 1,		//Дискретный
	mptAnalog = 2,			//Аналоговый
	mptNumeral = 3			//Числовой ввод или вывод(например, количество импульсов или ШИМ) 	
};

enum PinType
{
	ptPin = 0,
	ptPout = 1
};


_PACKED_BEGIN

typedef struct IO_MODULE
    {
    WORD	NumFirstParam;
    WORD	QuanParams;
    char*	ModuleName;
    WORD	ModuleType;
#ifdef IO_TASK_SIMPLE_MODULE
    BYTE	ModulePinType;
    BYTE	ModuleScaleId;
    BYTE	PinType;
    BYTE*	Scale;
#endif
#if !defined(IO_TASK_SIMPLE_MODULE) || defined(USE_MFC3000_ON_MFC5823)
    BYTE	QuanAI;
    BYTE	QuanAO;
    BYTE	QuanDI;
    BYTE	QuanDO;
    DWORD	FlagsDO;
#endif
    BYTE	Address;
    BYTE	TypeAnalogParams;
    BYTE	InUse; // TRUE - found in controller, FALSE - not found
    WORD	ErrorsInCurCycle;
    WORD	ErrorReadingCount;
    WORD	ErrorWritingCount;
    WORD	DataRange[16];	// Data Range Specifier For Analog Inputs/Outputs (currently used in ADAM5510 IO module)
	BYTE*	Property;

#ifdef PC_ADAM
	WORD	DataRangeChannel[8][8];
#endif

#if defined(LINUX_MFC)
    mfc_cpi*	drv_in;		// Driver for module
    mfc_cpi*	drv_out;	// Driver for module
    mfc_cpi*	drv_numeral;	// Driver for PWN or FI module
    int		Data2[16];
#endif
    } R_PACKED IO_MODULE;

_PACKED_END

_PACKED(typedef struct IO_TASK_CB
    {
    UNALIGNED TASK_HEADER_EX PTR pHeader;
    UNALIGNED TASK_PARAM  PTR pListParams;
	UNALIGNED SCALE_PARAM PTR pScaleParams;
    RPARAM PTR pParams;
	RPARAM PTR pOutParams;
    WORD  QuanModules;
	WORD  QuanScaleParams;
    IO_MODULE PTR pModules;
    DWORD             TimeNextStep_msec;
    DWORD             LenLastStep_msec;
    DWORD             StartLastStep_msec;
    DWORD             cicle_counter;
    DWORD             error_counter;
	int	GlobalArrayChangeCounter;
	RBOOLEAN8	bWriteFault;
	RBOOLEAN8	bReadFault;
    })  IO_TASK_CB;

EXTERN_C RHANDLE IOTaskRPID;
EXTERN_C IO_TASK_CB IOTaskCB;
EXTERN_C int IOTaskPresent;

#ifdef MTCORE_PROC
    EXTERN_C void IOTaskThread(PID *pid, WORD Task);
#elif defined(PC_WIN32)
    DWORD IOTaskThread(LPVOID lpThreadParameter);
#elif defined(LINUX)
    DWORD IOTaskThread(void *lpThreadParameter);
#elif defined(ECOS)
    void IOTaskThread(void *lpThreadParameter);
#endif

EXTERN_C int  InitIOTask( WORD Task );
EXTERN_C int	ReadControllerType(void);

#define LED_FAULT		0x1
#define LED_TEST		0x2
#define LED_ACTIVE_OUT	0x4   //Контроллер является мастером
#define LED_ERROR		0x8
#define LED_RUNNING		0x10
#define LED_COMM		0x20
#define LED_NOT_ANSWER	0x40
#define LED_TRY_SWITCH	0x80

#define RED_NOT_ANSWER_FAULT_LEVEL	0xFFFF

#define RUNNING_STATE_START_PAUSE 0x1
#define RUNNING_STATE_STOP 0x2
#define RUNNING_STATE_NETWORK 0x4
#define RUNNING_STATE_NO_CONFIG 0x8
#define RUNNING_STATE_ALL 0xff

//#define STATUS_RESERV	0x1   //Текущий контроллер в режиме резерва
//#define STATUS_BLOCK_OUT	0x2
//#define STATUS_OTHER_MASTER	0x4  //Другой контроллер мастер

EXTERN_C void SetLedStatus(int mask, int status);
EXTERN_C void SetRunningStatus(int mask, int status);
EXTERN_C int GetRunningStatus();
EXTERN_C int GetLedStatus(void);
EXTERN_C uint32_t GetFaultLevel(void);
EXTERN_C void SetFaultLevel(uint32_t level);
EXTERN_C int GetProjectSessionId(void);

#if defined(PLC_DRV_FUNC)
	EXTERN_C void IOUpdateLedStatus(int mask, int status);
	EXTERN_C int GetControllerStatus(void);
	EXTERN_C void DoDriverEndTask(void);
#else
	#define	IOUpdateLedStatus(mask, status)
	#define	GetControllerStatus()	0
	#define	DoDriverEndTask(void)
#endif

EXTERN_C void InitIOTaskCommon(RBOOLEAN8 bAllocOutArray);
EXTERN_C void IOTaskFinish(void);
EXTERN_C void IOTaskThreadOneStep(WORD Task);
EXTERN_C void CanRunIOTaskWriteStep(void);
EXTERN_C void DoDriverTask(void);
EXTERN_C void SetDigitLed(int value);
EXTERN_C int DoDriverStartupTask(void);
EXTERN_C void SetReadingModuleFaultState(IO_MODULE PTR pModule, RBOOLEAN bFault, const char* error);
EXTERN_C void SetWritingModuleFaultState(IO_MODULE PTR pModule, RBOOLEAN bFault, const char* error);

_PACKED(typedef struct RESOURCE_TASK_CB
{
	UNALIGNED TASK_HEADER_EX PTR pHeader;
	UNALIGNED TASK_PARAM  PTR pListParams;
	UNALIGNED RPARAM PTR pParams;
})  RESOURCE_TASK_CB;

enum ResourceParamIDStatistic
{
	rpidStatPeriodUser0 = 0,
	rpidStatPeriodSerial0 = 6,
	rpidStatPeriodIO = 10,
	rpidStatPeriodUDP = 11,
	rpidStatPeriodModbus = 12,
	rpidStatPeriodOtherContr = 13,
	rpidStatPeriodHotRestart = 14,
	rpidStatPeriodWDT = 15,
	rpidStatPeriodReserv = 16,
	rpidStatPeriodDiag = 17,
	rpidStatPeriodTelnet = 18,
	rpidStatTerminal = 19
};

enum ResourceParamIDStatisticEx
{
	rpidStatPeriodArchive = 0
};

enum ResourceParamID
{
	rpidModemReset = 1,
	rpidModemCall = 2,
	rpidProgramFault = 3,
	rpidStateFault = 4,
	rpidStateReserv = 5,
	rpidSaveData = 6,
	rpidReservNumber = 7,
	rpidProgramError = 8,
	rpidStateError = 9,
	rpidReservBlockControl = 10,
	rpidReservPeriod = 11,
	rpidStatus = 12,
	rpidRestart = 13,
    rpidFaultLevel = 14,
	rpidIgnoreFault = 15,
	rpidReservOtherStatus = 16,
    rpidReservOtherFaultLevel = 17,
	rpidSystemTimeCorrectionSec = 18,
	rpidEnableTrace = 19,
	rpidStatPeriod = 20,
	rpidStatWorkTime = 40,
	rpidSyncScan = 60,
	rpidStatPeriodEx = 80,
	rpidStatWorkTimeEx = 90
};

//enum ControllerStateFlags
#define	csfRestarting  0x0001
#define	csfInReserv  0x0002
#define	csfFailure  0x0004 // Установлен признак «Отказ»
#define	csfHasError  0x0008// Установлен признак «Ошибка»
#define	csfProgramFailure  0x0010 //Установлен признак "Програмный отказ"
#define	csfManualProgramFailure  0x0020 //Установлен признак "Ручное управление програмным отказом"
#define	csfConfigDiffer  0x0040// - Конфигурация отличается
#define	csfRTConfigEquals  0x0080// - RT Конфигурация совпадает
#define	csfInternalModuleFault  0x0100 
#define	csfExternalModuleFault  0x0200
#define	csfConnectionWithAnotherControllerFault  0x0400 
#define	csfHandInhibitionOfControl  0x0800
#define	csfConnectionWithUpperLevelFault  0x1000
//#define	csfCanAccessParamsByID  0x00010000 //Возможно чтение параметров по ID
//#define	csfUse8ByteParam      0x00020000
#define csfIgnoreFailure      0x00040000
#define csfDemoVersion        0x00080000
#define csfNeedActivation     0x00100000 //Данную копию MasterPLC необходимо активировать
#define csfUseConvNumbers      0x00200000 //Контроллер имеет архитектуру BigEndian или PLCOWEN
#define csfImitationMode      0x00400000 //Контроллер в режиме имитации (без доступа к портам и работы драйверов)
#define csfDemoMode		      0x00800000 //Контроллер в демо режиме (без доступа к портам и работы драйверов)
#define csfPLCUniversalBuild  0x01000000 //Возможно обновление версии через среду разработки
#define csfHasNewErrors		  0x02000000 //Появились новые ошибки программ
#define csfNoConnection       0x10000000
#define csfTryToSwitch		  0x20000000
#define csfNewMonitor		  0x40000000 // mplc запущен монитором

EXTERN_C RESOURCE_TASK_CB ResourceTaskCB;
EXTERN_C void UpdateFaultStatus(void);
EXTERN_C int GetFaultStatus(void);
EXTERN_C int InModulesImitation;
EXTERN_C void msatof(char *str,RFLOAT PTR floatVal);

#define INC_ERR_READING_COUNT(a) \
{ \
	if (a->ErrorReadingCount < MAX_ERRORS_TO_FAULT) \
		a->ErrorReadingCount++; \
}

#define SET_FAULT_ERR_READING(a) \
{ \
		a->ErrorReadingCount = MAX_ERRORS_TO_FAULT; \
}


#define SET_FAULT_ERR_WRITING(a) \
{ \
		a->ErrorWritingCount = MAX_ERRORS_TO_FAULT; \
}

#define SET_FAULT_ERR_ALL(a) \
{ \
		a->ErrorReadingCount = a->ErrorWritingCount = MAX_ERRORS_TO_FAULT; \
}


EXTERN_C void INC_ERR_WRITING_COUNT(DRV_MODULE PTR a);

void SetErrorOnParams(DRV_MODULE * pModule, int start, int num, WORD qcode);


#endif // #ifndef DRV_H
