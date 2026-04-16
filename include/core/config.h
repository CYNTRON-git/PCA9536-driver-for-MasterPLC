#pragma once
#pragma warning(disable : 4013 4018 4022 4024 4047 4101 4244 4267 4305 4430 4482 4838 4996 5105)
#pragma conform(forScope, off)
// C4018: '>=' : signed/unsigned mismatch
// C4244: 'conversion' conversion from 'type1' to 'type2', possible loss of data
// C4267: 'initializing' : conversion from 'size_t' to 'int', possible loss of data
// C4305 'initializing' : truncation from 'const int ' to 'char '
// C4022: 'OpcUa_P_RawSocket_FD_Isset': pointer mismatch for actual parameter 1
#if defined(LINUX)
#    include <stdio.h>  //for __WORDSIZE
#endif

#ifdef __cplusplus
#    ifndef EXTERN_C
#        define EXTERN_C extern "C"
#    endif
#    define BEGIN_EXTERN_C extern "C" {
#    define END_EXTERN_C }
#else
#    ifndef EXTERN_C
#        define EXTERN_C extern
#    endif
#    define BEGIN_EXTERN_C
#    define END_EXTERN_C
#endif

// НЕ извесныйы дефайн удалить вместе с файлом
// #ifdef MASTER_LINK
// #    include "masterlink.h"
// #endif

#ifndef MASTERSCADA_SYSTEM_CODE
#    define MASTERSCADA_SYSTEM_CODE 0xa4f345be  // Демо
#endif

// General platforms
#define PLATFORM_WIN32 1
#define PLATFORM_WINCE 2
#define PLATFORM_LINUX_X86 3
#define PLATFORM_QNX 4
#define PLATFORM_ELBRUS 5
#define PLATFORM_ACTIVATION_TIMED 6
#define PLATFORM_LINUX_KEY 7
#define PLATFORM_WIN64 8
#define PLATFORM_ELBRUS_X64 9
#define PLATFORM_LINUX_X64 10
#define PLATFORM_LINUX_X64_KEY 11
#define PLATFORM_LINUX_ARMV7HF 12
#define PLATFORM_LINUX_ARMV8 14

// Specific CE/win platforms
#define PLATFORM_WINPAC8000 51
#define PLATFORM_WINPAC9000 52
#define PLATFORM_XPAC8000 53

// Specific linux platforms
#define PLATFORM_ADAM3600 101
#define PLATFORM_BOLID 102
#define PLATFORM_BTUNE 103
#define PLATFORM_FASTWELL_MK150 104
#define PLATFORM_PLC110M2 105
#define PLATFORM_RASPBERRY 106
#define PLATFORM_TION_28 107
#define PLATFORM_WAGO750 108
#define PLATFORM_WIRENBOARD5 109
#define PLATFORM_WIRENBOARD6 110
#define PLATFORM_TREI_915_LINUX 111
#define PLATFORM_MOXA 112
#define PLATFORM_SEREBRUM_IRIS 113
#define PLATFORM_OPTILOGIC 114
#define PLATFORM_PLC210 115
#define PLATFORM_TION_PRO 116
#define PLATFORM_OSATEC 117
#define PLATFORM_OSATEC_X64 118
#define PLATFORM_ELPK_X64 119
#define PLATFORM_ERIS 120
#define PLATFORM_BAIKALM 121
#define PLATFORM_FIMATIC_C 122
#define PLATFORM_BOLID_M3000T_V2 123
#define PLATFORM_BAIKAL_T 124
#define PLATFORM_ABAK_K2 125
#define PLATFORM_TREI_500 126
#define PLATFORM_KVANTOR 127
#define PLATFORM_WIRENBOARD7 128
#define PLATFORM_ONI_PLC_W 129
#define PLATFORM_BAGET 130
#define PLATFORM_TITAN 131
#define PLATFORM_TITAN2000 132
#define PLATFORM_NLSCONRSB 133
#define PLATFORM_ABAK_K3 134
#define PLATFORM_UZOLA 135
#define PLATFORM_AVRORA 136
#define PLATFORM_PLC210RK 137
#define PLATFORM_NLSCONA40 138
#define PLATFORM_PLC_STABUR 139
#define PLATFORM_BAGETPLC1 140
#define PLATFORM_PLC_INBRES 141
#define PLATFORM_PLC_EUROPRIBOR 142

// Specific QNX platforms
#define PLATFORM_REGUL 201
#define PLATFORM_TREI_915 202
#define PLATFORM_TREI_903 203

#if defined(PLC_NEED_ACTIVATION) && defined(PLC_RESTRICT_END_DATE)
#    define TARGET_PLATFORM_ID PLATFORM_ACTIVATION_TIMED
#elif defined(PLC_ERIS)
#    define TARGET_PLATFORM_ID PLATFORM_ERIS
#elif defined(PLC_ADAM3600)
#    define TARGET_PLATFORM_ID PLATFORM_ADAM3600
#elif defined(PLC_BOLID)
#    define TARGET_PLATFORM_ID PLATFORM_BOLID
#elif defined(PLC_BOLID_M3000T_V2)
#    define TARGET_PLATFORM_ID PLATFORM_BOLID_M3000T_V2
#elif defined(PLC_BTUNE)
#    define TARGET_PLATFORM_ID PLATFORM_BTUNE
#elif defined(PLC_FASTWELL_MK150)
#    define TARGET_PLATFORM_ID PLATFORM_FASTWELL_MK150
#elif defined(PLC110)
#    define TARGET_PLATFORM_ID PLATFORM_PLC110M2
#elif defined(PLC210)
#    define TARGET_PLATFORM_ID PLATFORM_PLC210
#elif defined(PLC210RK)
#    define TARGET_PLATFORM_ID PLATFORM_PLC210RK
#elif defined(PLC_RASPBERRY)
#    define TARGET_PLATFORM_ID PLATFORM_RASPBERRY
#elif defined(PLC_TION_28)
#    define TARGET_PLATFORM_ID PLATFORM_TION_28
#elif defined(PLC_WAGO750)
#    define TARGET_PLATFORM_ID PLATFORM_WAGO750
#elif defined(PLC_WIRENBOARD5)
#    define TARGET_PLATFORM_ID PLATFORM_WIRENBOARD5
#elif defined(PLC_WIRENBOARD6)
#    define TARGET_PLATFORM_ID PLATFORM_WIRENBOARD6
#elif defined(PLC_KVANTOR)
#    define TARGET_PLATFORM_ID PLATFORM_KVANTOR
#elif defined(PLC_WIRENBOARD7)
#    define TARGET_PLATFORM_ID PLATFORM_WIRENBOARD7
#elif defined(PLC_ONI_W)
#    define TARGET_PLATFORM_ID PLATFORM_ONI_PLC_W
#elif defined(PLC_BAGET)
#    define TARGET_PLATFORM_ID PLATFORM_BAGET
#elif defined(PLC_TITAN)
#    define TARGET_PLATFORM_ID PLATFORM_TITAN
#elif defined(PLC_TREI_915) && defined(QNX_NEUTRINO)
#    define TARGET_PLATFORM_ID PLATFORM_TREI_915
#elif defined(PLC_TREI_915)
#    define TARGET_PLATFORM_ID PLATFORM_TREI_915_LINUX
#elif defined(PLC_MOXA)
#    define TARGET_PLATFORM_ID PLATFORM_MOXA
#elif defined(PLC_SEREBRUM)
#    define TARGET_PLATFORM_ID PLATFORM_SEREBRUM_IRIS
#elif defined(PLC_OPTILOGIC)
#    define TARGET_PLATFORM_ID PLATFORM_OPTILOGIC
#elif defined(PLC_CHGP_RT) && (__WORDSIZE == 64)
#    define TARGET_PLATFORM_ID PLATFORM_OSATEC_X64
#elif defined(PLC_CHGP_RT)
#    define TARGET_PLATFORM_ID PLATFORM_OSATEC
#elif defined(PLC_ELPK_X64)
#    define TARGET_PLATFORM_ID PLATFORM_ELPK_X64
#elif defined(PLC_TREI_903)
#    define TARGET_PLATFORM_ID PLATFORM_TREI_903
#elif defined(PLC_REGUL)
#    define TARGET_PLATFORM_ID PLATFORM_REGUL
#elif defined(PLC_WINPAC_9000)
#    define TARGET_PLATFORM_ID PLATFORM_WINPAC9000
#elif defined(PLC_WINPAC)
#    define TARGET_PLATFORM_ID PLATFORM_WINPAC8000
#elif defined(PLC_XPAC8000)
#    define TARGET_PLATFORM_ID PLATFORM_XPAC8000
#elif defined(PLC_E2K) && (__WORDSIZE == 64)
#    define TARGET_PLATFORM_ID PLATFORM_ELBRUS_X64
#elif defined(PLC_E2K)
#    define TARGET_PLATFORM_ID PLATFORM_ELBRUS
#elif defined(PLC_TION_PRO)
#    define TARGET_PLATFORM_ID PLATFORM_TION_PRO
#elif defined(PLC_BAIKALM)
#    define TARGET_PLATFORM_ID PLATFORM_BAIKALM
#elif defined(PLC_BAIKAL_T)
#    define TARGET_PLATFORM_ID PLATFORM_BAIKAL_T
#elif defined(PLC_FIMATIC_C)
#    define TARGET_PLATFORM_ID PLATFORM_FIMATIC_C
#elif defined(PLC_ABAK_K2)
#    define TARGET_PLATFORM_ID PLATFORM_ABAK_K2
#elif defined(PLC_ABAK_K3)
#    define TARGET_PLATFORM_ID PLATFORM_ABAK_K3
#elif defined(PLC_TREI_500)
#    define TARGET_PLATFORM_ID PLATFORM_TREI_500
#elif defined(PLC_LINUX_ARMV8)
#    define TARGET_PLATFORM_ID PLATFORM_LINUX_ARMV8
#elif defined(PLC_LINUX_ARMV7HF)
#    define TARGET_PLATFORM_ID PLATFORM_LINUX_ARMV7HF
#elif defined(PLC_TITAN2000)
#    define TARGET_PLATFORM_ID PLATFORM_TITAN2000
#elif defined(PLC_UZOLA)
#    define TARGET_PLATFORM_ID PLATFORM_UZOLA
#elif defined(PLC_AVRORA)
#    define TARGET_PLATFORM_ID PLATFORM_AVRORA
#elif defined(PLC_NLSCONRSB)
#    define TARGET_PLATFORM_ID PLATFORM_NLSCONRSB
#elif defined(NLS_CON_A40)
#    define TARGET_PLATFORM_ID PLATFORM_NLSCONA40
#elif defined(BAGETPLC1)
#    define TARGET_PLATFORM_ID PLATFORM_BAGETPLC
#elif defined(PLC_INBRES)
#    define TARGET_PLATFORM_ID PLATFORM_PLC_INBRES
#elif defined(PLC_EUROPRIBOR)
#    define TARGET_PLATFORM_ID PLATFORM_PLC_EUROPRIBOR
#elif defined(PLC_STABUR)
#    define TARGET_PLATFORM_ID PLATFORM_PLC_STABUR
#elif defined(LINUX) && (defined(INSAT_GUARD_GUARDANT) || defined(INSAT_GUARD_GUARDANT_NET)) && (__WORDSIZE == 64)
#    define TARGET_PLATFORM_ID PLATFORM_LINUX_X64_KEY
#elif defined(LINUX) && (__WORDSIZE == 64)
#    define TARGET_PLATFORM_ID PLATFORM_LINUX_X64
#elif defined(LINUX) && (defined(INSAT_GUARD_GUARDANT) || defined(INSAT_GUARD_GUARDANT_NET))
#    define TARGET_PLATFORM_ID PLATFORM_LINUX_KEY
#elif defined(LINUX)
#    define TARGET_PLATFORM_ID PLATFORM_LINUX_X86
#elif defined(WINCE)
#    define TARGET_PLATFORM_ID PLATFORM_WINCE
#elif defined(_WIN64)
#    define TARGET_PLATFORM_ID PLATFORM_WIN64
#elif defined(_M_IX86) && defined(_WIN32)
#    define TARGET_PLATFORM_ID PLATFORM_WIN32
#elif defined(QNX_NEUTRINO)
#    define TARGET_PLATFORM_ID PLATFORM_QNX

#else
#    error "Platform not defined"
#endif

// PLC options
// #define PLC_OPTION_DEMO
// #define PLC_OPTION_RESERV
#ifndef PLC_OPTION_ARCHIVE
#    define PLC_OPTION_ARCHIVE
#endif

#ifndef PLC_OPTION_HOTRESTART
#    define PLC_OPTION_HOTRESTART
#endif

#ifndef PLC_RT_LIMITED
#    ifndef USE_MODEM
#        define USE_MODEM
#        define PLC_OPTION_MODEM_INITCALL
#        define PLC_OPTION_SMS
#    endif


#    if !defined(PLC_OPTION_EMAIL)
#        define PLC_OPTION_EMAIL
#    endif
#endif

#if (defined(USE_MODEM) || defined(PLC_OPTION_SMS)) && !defined(USE_MODEM_EXT_REQUEST)
#    define USE_MODEM_EXT_REQUEST
#endif

#define PLC_OPTION_IMITATION

#ifndef USE_MERCURY230
#    define USE_MERCURY230
#endif

#define PLC_OPTION_MODBUS_RTU_SLAVE
#ifndef PLC_OPTION_MODBUS_TCP_MASTER
#    define PLC_OPTION_MODBUS_RTU_MASTER
#    define PLC_OPTION_MODBUS_UDP_MASTER
#    define PLC_OPTION_MODBUS_TCP_MASTER
#endif

#if !defined(PC_WIN32) && (defined(WINCE) || defined(_WIN32) || defined(_WIN64))
#    define PC_WIN32
#endif

#if !defined(PLCOWEN) && (defined(PLC100) || defined(PLC304) || defined(PLC110) || defined(PLC210) || defined(PLC308))
#    define PLCOWEN
#endif

#if defined(PLC110)
#    define PLC_OEM_DEFINE_FEAUTURES
#endif

#if defined(PLC110) || defined(PLC210)
#    define PLC_DRV_USE_TASK_FUNC
#    undef PLC_OPTION_REPORTS
#    undef PLC_OPTION_DB
#else
#    ifndef PLC_OPTION_REPORTS
#        define PLC_OPTION_REPORTS
#    endif
#endif

#if defined(PC_WIN32) && !defined(WINCE)
#    define PC_WIN32_ONLY
#endif

#if defined(PLCANDR) || defined(LINUX) || defined(WINCE)
#    define NEEDALIGNED
#endif

#define THREAD_PRIORITY_DEFAULT 100
#define THREAD_PRIORITY_UDP THREAD_PRIORITY_DEFAULT
#define THREAD_PRIORITY_TELNET THREAD_PRIORITY_DEFAULT
#define THREAD_PRIORITY_MODBUS THREAD_PRIORITY_DEFAULT
#define THREAD_PRIORITY_HOT_RESTART THREAD_PRIORITY_DEFAULT
#define THREAD_PRIORITY_ARCHIVE THREAD_PRIORITY_DEFAULT
#define THREAD_PRIORITY_TERMINAL THREAD_PRIORITY_DEFAULT
#define THREAD_PRIORITY_DIAG THREAD_PRIORITY_DEFAULT
#define THREAD_PRIORITY_RESERV THREAD_PRIORITY_DEFAULT
#define THREAD_PRIORITY_MAIN THREAD_PRIORITY_DEFAULT

#define THREAD_PRIORITY_WDT 120
#define MIN_PAUSE_HIGH_PRIORITY 20  // In percents
#define MIN_PAUSE_DEFAULT 5         // In percents
#define MAX_ARCHIVE_TASK_COUNT 4

#define USE_FILESYSTEM
#define PLC_8BYTE_PARAM
#define USE_HASH_PARAMSID

#define MAX_QUAN_USER_TASKS 32767
#define TASK_WS_SIZE 2048L

#ifndef MAX_QUAN_SERIAL_TASKS
#    define MAX_QUAN_SERIAL_TASKS 32767
#endif

#ifndef SERIAL_RECEIVE_BUFFER_SIZE
#    define SERIAL_RECEIVE_BUFFER_SIZE 32 * 1024  // В COMPortFB тип MaxResponseSize INT
#endif

#ifndef SERIAL_SEND_BUFFER_SIZE
#    define SERIAL_SEND_BUFFER_SIZE 32 * 1024
#endif

#define USER_TASK_WS_SIZE TASK_WS_SIZE
#define SERIAL_TASK_WS_SIZE TASK_WS_SIZE

#ifndef DELETE_CONFIG_ON_FAULT_TIME
#    define DELETE_CONFIG_ON_FAULT_TIME 0  // Не удалять конфигурацию в MASTER
#endif

#ifndef SERIAL_MODULE_TIMEOUT
#    define SERIAL_MODULE_TIMEOUT 500L  // TODO Сделать настройку
#endif
#define SERIAL_MODULE_REQUEST_INTERVAL 5
#define UPPER_LEVEL_GETSTATUS_TIMEOUT 60000L

#define MY_UDP_PORT 30550
#define MY_FASTCGI_PORT 30750
#define MY_TELNET_PORT 31550

// Set options

// Version history
#define CONFIG_VERSION_HIST_START 0x0100013A  // Начальная версия

// В связь с внешними УСО флаг опрос в резерве Команды имеют 2 типа
#define CONFIG_VERSION_HIST_050606 0x0100013B
#define CONFIG_VERSION_HIST_060414 0x0100013C  // CONFIG_EXTRA_BLOCK
#define CONFIG_VERSION_HIST_060619 0x0100013D  // RC_SET_RT_CONFIG_ID
#define CONFIG_VERSION_HIST_060815 0x0100013E  // RC_GET_ARCHIVE_EX
#define CONFIG_VERSION_HIST_071227 0x0100013F  // RC_SET_TIME extension

#define CONFIG_MIN_VERSION_FULL (0x00010000 * (0x1000 * 1 + 0x0100 * 3 + 4))
#define CONFIG_CURRENT_VERSION_MAIN() ((WORD)(0x1000 * rmj + 0x0100 * rmm + rmm2))
#define CONFIG_CURRENT_VERSION_FULL() (0x00010000 * CONFIG_CURRENT_VERSION_MAIN() + rup)

#define USE_DISPLAY

#if (defined(_WIN32) || defined(LINUX)) && !defined(WINCE)
#    define USE_RESERV_TASK
#endif

#if !defined(USE_MODEM) && (defined(USE_MODEM_EXT_REQUEST) || defined(PLC_OPTION_SMS))
#    define USE_MODEM
#endif

#if (defined(PC_MSDOS) || defined(_WIN32) || defined(LINUX) || defined(TKM410) || defined(IPC_ANY)) &&                 \
    defined(PLC_OPTION_HOTRESTART)
#    define USE_HOTRESTART_TASK
#endif

#define USE_EXTACCESS_TASK  // Внешний доступ через OPC разрешен всегда, доступ по Modbus - через
                            // опцию PLC_OPTION_MODBUS_RTU_SLAVE

#ifndef MAX_OTHER_CONTROLLERTASKS
#    if defined(PC_WIN32) || defined(LINUX)
#        define MAX_OTHER_CONTROLLERTASKS 5
#    else
#        define MAX_OTHER_CONTROLLERTASKS 1
#    endif
#endif

#define ENABLE_WATCHDOG
// #define TRACE_GLOBAL_ARRAY

#define NO_MAX_OPTIMIZATION
#define TRACE_SECOND_TICKS

#if defined(TKM410) || defined(MFC3000) || defined(USE_MFC3000_ON_MFC5823)
#    define IO_TASK_SIMPLE_MODULE
#endif

#if defined(TKM410) || defined(_WIN32) || defined(LINUX_DAEMON)
#    define USE_TELNET
#endif

#define INSAT_MAX_FB_TYPE 200
#define INSAT_MAX_DRIVER_TYPE 200

#if defined(PC_MFC) || defined(PC_LOM) || defined(PLC_WINPAC) || defined(PLC_WINPAC_9000) || defined(PLC_LINPAC) ||    \
    defined(I8000) || defined(I8000E) || defined(I8KE80) || defined(PC_ADAM) || defined(LINUX_MFC) ||                  \
    defined(TKM410) || defined(PLC_XPAC8000) || defined(PLC110) || defined(PLC210) || defined(PLC_TREI_903) ||         \
    defined(PLC_TREI_915)
#    define USE_IO_TASK
#endif

#define USE_UDP_TASK

#if !defined(LINUX_DAEMON) && !defined(PLC_WINPAC)
#    define ENABLE_RESTART_CONFIG
#endif

#if defined(PC_WIN32) || defined(LINUX) || defined(USE_MODEM) ||                                                       \
    ((defined(IPC_ANY) || defined(PC_ADAM)) && !defined(USE_UDP_TASK))
#    define USE_MODBUS_SLAVE_SERIAL_TASK
#endif

#define PLC_OPTION_MODBUS_RTU_SLAVE
#define USE_MODBUS_SLAVE_TCP_TASK
#ifndef USE_ADDINS
#    define USE_ADDINS
#endif

#if !defined(PLC_DRV_FUNC) &&                                                                                          \
    (defined(PC_MFC) || defined(PC_LOM) || defined(LINUX_MFC) || defined(PLC_WINPAC) || defined(PLC_LINPAC) ||         \
     defined(PLCOWEN) || defined(UC7110) || defined(UC7408LX) || defined(PLC3250) || defined(PLCANDR) ||               \
     defined(TECONP06) || defined(PLC_BTUNE) || defined(PLC_ADAM3600) || defined(PLC_XPAC8000) ||                      \
     defined(PLC_BOLID) || defined(PLC_BOLID_M3000T_V2) || defined(PLC_TREI_915) || defined(PLC_TREI_903) ||           \
     defined(PLC_WINPAC_9000) || defined(PLC_WAGO750))
#    define PLC_DRV_FUNC
#endif

#if defined(LINUX) && !defined(ANDROID) && !defined(__CYGWIN__)
#    define USE_ADJUST_TIME
#endif

#define LUADLL
#if defined(WIN32) && !defined(PLC_XPAC8000)
#    define LUA  // На linux платформы этот  define ставится в makefile
#endif

#if !defined(PLC100)
#    define USE_OPCUA
#endif

#if defined(PLC100) || defined(PLC_LINPAC)
#    define LIMIT_UDP_PACKET 1024
#elif defined(PLC110)
#    define LIMIT_UDP_PACKET 1400
#endif

#define WDT_TASK_CANT_STOP

// #define TREI_EMUL 1

#ifndef MASTERPLC_API
#    if defined(WIN32) || defined(WINCE) || defined(X64) || defined(_WIN64)
#        ifdef MASTERPLC_EXPORTS
#            define MASTERPLC_API __declspec(dllexport)
#        else
#            define MASTERPLC_API __declspec(dllimport)
#        endif
#    else
#        define MASTERPLC_API
#    endif
#endif

#if defined(__RESHARPER__)
#    define PRINTF_ATTR(StrIdx, FCheck) [[rscpp::format(printf, StrIdx, FCheck)]]
#else
#    define PRINTF_ATTR(StrIdx, FCheck)
#endif
#ifndef _Printf_format_string_
#    define _Printf_format_string_
#endif

#ifdef _WIN32
#    define ENABLE_MINIDUMP
#endif
