// File CommonStructs.h
#ifndef _COMMONSTRUCTS
#define _COMMONSTRUCTS

//Константы для признаков качества
#include "opcaedef.h"
#include <atlbase.h>

//'identifier' : identifier was truncated to 'number' characters in the debug information
#pragma warning(disable : 4786)
// decorated name length exceeded, name was truncated
#pragma warning(disable : 4503)

// MasterSACDA namespace
namespace MS {
    enum HostItemType { hitFB = 0, hitObject = 1, hitOPC = 2, hitAttribute = 3, hitEvent = 4 };

    enum ELanguageIndex { liRussian = 0, liEnglish = 1 };

    inline ELanguageIndex GetLanguageIndex(DWORD localeId) {
        return PRIMARYLANGID(localeId) == LANG_ENGLISH ? liEnglish : liRussian;
    }

    //функции для извлечения интервалов и единиц измерения
    inline int GET_INTERVAL(DWORD dwValidity) {
        return ((dwValidity & 0xFFFF0000) == 0xFFFF0000) ? 0 : HIWORD(dwValidity);
    }

    inline DWORD ComposeValidity(int nInterval, WORD wValidity) {
        if (nInterval == -1)
            nInterval = 0;
        return (((WORD)nInterval) << 16) | wValidity;
    }

    inline WORD GetValidity(DWORD dwValidity) {
        return LOWORD(dwValidity);
    }

    inline void SetOPCValidity(DWORD& dwValidity, WORD wOPCValidity) {
        dwValidity = (dwValidity & ~OPC_STATUS_MASK) | (wOPCValidity & OPC_STATUS_MASK);
    }

    const DWORD OPC_MASK = OPC_STATUS_MASK;
    const DWORD CONTROL_MASK = 0x00001F00;

    inline WORD GetOPCValidity(DWORD dwValidity) {
        return (WORD)(dwValidity & OPC_MASK);
    }

    inline DWORD ClearValidityInterval(DWORD dwValidity) {
        return GetValidity(dwValidity) & ~CONTROL_MASK;
    }

    inline bool IS_OPC_GOOD(DWORD dwValidity) {
        return (dwValidity & OPC_QUALITY_MASK) == OPC_QUALITY_GOOD;
    }

    inline bool IsValueExists(DWORD dwValidity) {
        return (dwValidity & OPC_MASK) != OPC_QUALITY_COMM_FAILURE;
    }

    //константы для сообщений выхода ФБ
    const DWORD PC_MAX_SPEED = 0x00000100;
    const DWORD PC_MIN_WARNING = 0x00000200;
    const DWORD PC_MIN_ERROR = 0x00000400;
    const DWORD PC_MAX_WARNING = 0x00000800;
    const DWORD PC_MAX_ERROR = 0x00001000;

    //константа ошибки в работе ФБ (передается в SetPoutData)
    const DWORD FB_ERROR = OPC_QUALITY_DEVICE_FAILURE;
    //команда
    const DWORD LAST_SESSION_VALUE = 0x00002000;
    const DWORD NOT_ACTUAL_VALUE = 0x00004000;  // If data is not actual
    const DWORD COMMAND_VALUE = 0x00008000;

    inline DWORD timeDifference(DWORD dwTime1, DWORD dwTime2) {
#ifdef _DEBUG
        if (dwTime1 < dwTime2 && (dwTime2 - dwTime1) < (24 * 60 * 60000)) {
            CString str;
            str.Format("Invalid use timeDifference(%d, %d) diff = %d", dwTime1, dwTime2, dwTime2 - dwTime1);
            //_ASSERT_EXPR(0, CComBSTR(str));
        }
#endif
        return (dwTime1 >= dwTime2) ? dwTime1 - dwTime2 : dwTime1 + (0xFFFFFFFF - dwTime2);
    }

    inline bool timeGreater(DWORD dwTime1, DWORD dwTime2) {
        DWORD dwDiff = (dwTime1 >= dwTime2) ? dwTime1 - dwTime2 : dwTime1 + (0xFFFFFFFF - dwTime2);
        return dwDiff < 0x7FFFFFFF;
    }

    inline DWORD timeAdd(DWORD dwTime1, DWORD dwDelta) {
        return dwTime1 + dwDelta;
    }

    const FILETIME NullFileTime = {0, 0};
    const FILETIME MinFileTime = {0xCAAB8800, 0x1A8E7C9};  //~01/01/1980
    const FILETIME FutureFileTime = {0, 0x20000000};       //~12/31/9999
    const LONGLONG g_lFileTimeSeconds = 10000000L;
    const LONGLONG g_lFileTimeMiliseconds = 10000L;

    inline bool operator==(const FILETIME& f1, const FILETIME& f2) {
        return *((ULONGLONG*)&f1) == *((ULONGLONG*)&f2);
    }
    inline bool operator!=(const FILETIME& f1, const FILETIME& f2) {
        return *((ULONGLONG*)&f1) != *((ULONGLONG*)&f2);
    }
    inline bool operator<(const FILETIME& f1, const FILETIME& f2) {
        return *((ULONGLONG*)&f1) < *((ULONGLONG*)&f2);
    }
    inline bool operator<=(const FILETIME& f1, const FILETIME& f2) {
        return *((ULONGLONG*)&f1) <= *((ULONGLONG*)&f2);
    }
    inline bool operator>(const FILETIME& f1, const FILETIME& f2) {
        return *((ULONGLONG*)&f1) > *((ULONGLONG*)&f2);
    }
    inline bool operator>=(const FILETIME& f1, const FILETIME& f2) {
        return *((ULONGLONG*)&f1) >= *((ULONGLONG*)&f2);
    }
    inline LONGLONG operator-(const FILETIME& f1, const FILETIME& f2) {
        return *((ULONGLONG*)&f1) - *((ULONGLONG*)&f2);
    }

    //Индекс текущего компьютера
    const int CURRENT_COMP = -2;

    enum DocumentType { DT_ACTIVEX, DT_PROPPAGE };
    enum ModuleType { MT_EDITOR, MT_ADDIN };

    typedef enum _UserMode {
        UM_Runtime,
        UM_Design,
        UM_PrepareRuntime,
        UM_RollbackRuntime,
        UM_Imitation,
        UM_PrepareImitation,
        UM_Debug,
        UM_PrepareDebug,
        UM_RollbackDebug
    } UserMode;

    enum TreeItemType {
        TIT_ROOTFOLDER = 0,
        TIT_FOLDER = 1,
        TIT_OBJECT = 2,
        TIT_PINGROUP = 3,
        TIT_POUTGROUP = 4,
        TIT_PIN = 5,
        TIT_POUT = 6,
        TIT_GROUP = 7,
        TIT_EVENTGROUP = 8,  // unused
        TIT_EVENT = 9,
        TIT_PARSER = 10,
        TIT_DELETED = 11,
        TIT_MASK_INTERNET_CLIENT_OBJECTS = 1 << 30,
        TIT_MASK_USE_OBJECT_TYPE = 1 << 31
    };
    // 0 -> 1, 1 -> 10, ...
    inline DWORD MASK(int type) {
        return (1 << type);
    }

#define GROUP_ITEM_MASK                                                                                                \
    (MS::MASK(TIT_PINGROUP) | MS::MASK(TIT_POUTGROUP) | MS::MASK(TIT_GROUP) | MS::MASK(TIT_EVENTGROUP))
#define VARIABLE_ITEM_MASK (MS::MASK(TIT_PIN) | MS::MASK(TIT_POUT) | MS::MASK(TIT_EVENT) | MS::MASK(TIT_PARSER))
#define CONTAINER_ITEM_MASK GROUP_ITEM_MASK | MASK(TIT_OBJECT) | MASK(TIT_FOLDER)

    inline bool ItemIsFolder(short tit) {
        return tit == TIT_ROOTFOLDER || tit == TIT_FOLDER;
    }

    inline bool ItemIsGroup(short tit) {
        return tit == TIT_PINGROUP || tit == TIT_POUTGROUP || tit == TIT_GROUP || tit == TIT_EVENTGROUP;
    }
    inline bool ItemIsPin(short tit) {
        return tit == TIT_PIN || tit == TIT_POUT || tit == TIT_EVENT || tit == TIT_PARSER;
    }

    inline bool ItemIsPoutType(short tit) {
        return tit == TIT_POUT || tit == TIT_EVENT || tit == TIT_PARSER;
    }

    inline bool ItemIsPinType(short tit) {
        return tit == TIT_PIN;
    }

    enum EActiveStatus { STATUS_ACTIVE, STATUS_NONACTIVE, STATUS_CHILDACTIVE, STATUS_PARENTACTIVE };

    inline LPCTSTR GetSeverityName(long nSeverity) {
        if (nSeverity > 950)
            return _T("Максимальный");
        else if (nSeverity > 850)
            return _T("Очень высокий");
        else if (nSeverity > 700)
            return _T("Высокий");
        else if (nSeverity > 400)
            return _T("Средний");
        else if (nSeverity > 150)
            return _T("Обычный");
        else
            return _T("Низкий");
    }
    inline int GetSeverityIndex(long nSeverity) {
        if (nSeverity > 950)
            return 5;
        else if (nSeverity > 850)
            return 4;
        else if (nSeverity > 700)
            return 3;
        else if (nSeverity > 400)
            return 2;
        else if (nSeverity > 150)
            return 1;
        else
            return 0;
    }

    const int EventsSeverityCounts = 6;

    inline long GetSeverityByIndex(int nIndex) {
        static const int inds[] = {100, 200, 500, 800, 900, 1000};
        return inds[nIndex];
    }

    inline DWORD GetChannelMask(long nChannel) {
        return 1 << nChannel;
    }

    // For IFastFBInternal::FastRequestData. lWave values :
    const int ALWAYS_ENABLE = -2;
    const int TEST_ENABLE = -1;
    const int UPDATE_ENABLE = 0;
    const int ADD_CONSUMER = -3;     // Add new consumer for fb data
    const int REMOVE_CONSUMER = -4;  // Remove consumer for fb data

    inline LPCTSTR GetVarTypeStr(VARTYPE varType) {
        switch (varType) {
        case VT_EMPTY:
            return _T("Нет данных");
        case VT_UI1:
            return _T("Беззнаковый короткий целый (один байт)");
        case VT_I1:
            return _T("Короткий целый (один байт)");
        case VT_UI2:
            return _T("Беззнаковый короткий целый");
        case VT_I2:
            return _T("Короткий целый");
        case VT_UINT:
        case VT_UI4:
            return _T("Беззнаковый целый");
        case VT_INT:
        case VT_I4:
            return _T("Целый");
        case VT_R4:
            return _T("Вещественный");
        case VT_R8:
            return _T("Вещественный дв. точности");
        case VT_BOOL:
            return _T("Логический");
        case VT_ERROR:
            return _T("Ошибка");
        case VT_CY:
            return _T("Деньги");
        case VT_DATE:
            return _T("Время");
        case VT_BSTR:
            return _T("Строковый");
        case VT_UNKNOWN:
            return _T("VT_UNKNOWN");
        case VT_DISPATCH:
            return _T("VT_DISPATCH");

        case VT_UI1 | VT_ARRAY:
            return _T("Массив : Беззнаковый короткий целый (один байт)");
        case VT_I1 | VT_ARRAY:
            return _T("Массив : Короткий целый (один байт)");
        case VT_UI2 | VT_ARRAY:
            return _T("Массив : Беззнаковый короткий целый");
        case VT_I2 | VT_ARRAY:
            return _T("Массив : Короткий целый");
        case VT_UINT | VT_ARRAY:
        case VT_UI4 | VT_ARRAY:
            return _T("Массив : Беззнаковый целый");
        case VT_INT | VT_ARRAY:
        case VT_I4 | VT_ARRAY:
            return _T("Массив : Целый");
        case VT_R4 | VT_ARRAY:
            return _T("Массив : Вещественный");
        case VT_R8 | VT_ARRAY:
            return _T("Массив : Вещественный дв. точности");
        case VT_BOOL | VT_ARRAY:
            return _T("Массив : Логический");
        case VT_ERROR | VT_ARRAY:
            return _T("Массив : Ошибка");
        case VT_CY | VT_ARRAY:
            return _T("Массив : Деньги");
        case VT_DATE | VT_ARRAY:
            return _T("Массив : Время");
        case VT_BSTR | VT_ARRAY:
            return _T("Массив : Строковый");
        case VT_UNKNOWN | VT_ARRAY:
            return _T("Массив : VT_UNKNOWN");
        case VT_DISPATCH | VT_ARRAY:
            return _T("Массив : VT_DISPATCH");
        }
        return _T("Неизвестен");
    }

    inline CString GetOPCQualityName(WORD wQuality, bool bShowGood = false, int li = MS::liRussian) {
        if (li == liEnglish) {
            if (IS_OPC_GOOD(wQuality))
                return bShowGood ? _T("Good") : _T("");

            switch (wQuality & OPC_MASK) {
            case OPC_QUALITY_BAD:
                return _T("Bad");
            case OPC_QUALITY_CONFIG_ERROR:
                return _T("Config error");
            case OPC_QUALITY_NOT_CONNECTED:
                return _T("Not connected");
            case OPC_QUALITY_DEVICE_FAILURE:
                return _T("Device failure");
            case OPC_QUALITY_SENSOR_FAILURE:
                return _T("Sensor failure");
            case OPC_QUALITY_LAST_KNOWN:
                return _T("Last known");
            case OPC_QUALITY_COMM_FAILURE:
                return _T("Comm failure");
            case OPC_QUALITY_OUT_OF_SERVICE:
                return _T("Out of service");

            case OPC_QUALITY_UNCERTAIN:
                return _T("Uncertain");
            case OPC_QUALITY_LAST_USABLE:
                return _T("Last usable");
            case OPC_QUALITY_SENSOR_CAL:
                return _T("Sensor Not Accurate");
            case OPC_QUALITY_EGU_EXCEEDED:
                return _T("Engineering Units Exceeded");
            case OPC_QUALITY_SUB_NORMAL:
                return _T("Sub-Normal");
            }
        } else {
            if (IS_OPC_GOOD(wQuality))
                return bShowGood ? _T("Норма") : _T("");

            switch (wQuality & OPC_MASK) {
            case OPC_QUALITY_BAD:
                return _T("Ошибка");
            case OPC_QUALITY_CONFIG_ERROR:
                return _T("Ошибка конфигурации");
            case OPC_QUALITY_NOT_CONNECTED:
                return _T("Нет соединения");
            case OPC_QUALITY_DEVICE_FAILURE:
                return _T("Ошибка устройства");
            case OPC_QUALITY_SENSOR_FAILURE:
                return _T("Ошибка датчика");
            case OPC_QUALITY_LAST_KNOWN:
                return _T("Последнее известное значение");
            case OPC_QUALITY_COMM_FAILURE:
                return _T("Нет связи");
            case OPC_QUALITY_OUT_OF_SERVICE:
                return _T("Не обслуживается");

            case OPC_QUALITY_UNCERTAIN:
                return _T("Не определено");
            case OPC_QUALITY_LAST_USABLE:
                return _T("Последнее достоверное значение");
            case OPC_QUALITY_SENSOR_CAL:
                return _T("Неточность датчика");
            case OPC_QUALITY_EGU_EXCEEDED:
                return _T("Превышение инженерных границ");
            case OPC_QUALITY_SUB_NORMAL:
                return _T("Sub-Normal");
            }
        }

        CString strMsg;
        strMsg.Format(_T("%#4X"), wQuality);
        return strMsg;
    }

    //#if (WINVER >= 0x0501)
    //
    // inline ELanguageIndex GetCurrentLanguageIndex()
    //{
    //	DWORD localeId = GetThreadUILanguage();
    //	return GetCurrentLanguageIndex(localeId);
    //}
    //
    //#endif

    template<class T>
    class CLockSection {
    public:
        CLockSection(T* pObject, bool bInitiallyOwn = true): m_pObject(pObject) {
            if (bInitiallyOwn) {
                m_pObject->Lock();
                m_bOwn = true;
            } else
                m_bOwn = false;
        }
        ~CLockSection() {
            if (m_bOwn)
                Unlock();
        }
        void Lock() {
            if (!m_bOwn)
                m_pObject->Lock();
            m_bOwn = true;
        }
        void Unlock() {
            if (m_bOwn)
                m_pObject->Unlock();
            m_bOwn = false;
        }
        T* m_pObject;

    private:
        bool m_bOwn;
    };

#if _ATL_VER >= 0x0700
    typedef CLockSection<ATL::CComCriticalSection> CComLockSection;
    typedef CLockSection<ATL::CComAutoCriticalSection> CComAutoLockSection;
#else
    typedef CLockSection<CComCriticalSection> CComLockSection;
    typedef CLockSection<CComAutoCriticalSection> CComAutoLockSection;
#endif

    inline HRESULT MakeOLESTR(LPCWSTR str, LPWSTR* ole_str) {
        if (str == NULL) {
            *ole_str = static_cast<LPWSTR>(::CoTaskMemAlloc(2));
            *ole_str[0] = 0;
        } else {
            *ole_str = static_cast<LPWSTR>(::CoTaskMemAlloc(2 * (::wcslen(str) + 1)));
            if (!*ole_str)
                return E_OUTOFMEMORY;
            ::wcscpy(*ole_str, str);
        }

        return S_OK;
    }

    static const GUID g_guidNumProps = {0x4f573d4f, 0x7628, 0x44a0, {0x9a, 0xd8, 0x8a, 0xeb, 0xd5, 0x66, 0xff, 0xa9}};
    inline DWORD MakeNumPropsKey(long nCategory, long nProp) {
        return MAKELONG(nCategory, nProp);
    }

    enum ECallType {
        ctAny = 0x0,
        ctOnlyDocument = 0x1,
        ctOnlyWindow = 0x2,
        ctDocument = 0x10,
        ctWindow = 0x20,
        ctNewWindow = 0x40,
        ctUseDefault = 0x80,
    };
    const int g_lMaskCallTypeRestriction = 0xF;
    const int g_lMaskCallType = 0xF0;

    inline int GetDefaultCallType(int nType) {
        if ((nType & g_lMaskCallTypeRestriction) == ctOnlyDocument)
            return ctOnlyDocument | ctDocument;

        if ((nType & ctOnlyWindow) && (nType & ctDocument))
            return ctOnlyWindow | ctNewWindow;

        if (nType & g_lMaskCallType)
            return nType;

        if (nType & ctOnlyWindow)
            return ctOnlyWindow | ctNewWindow;

        return ctDocument;
    }

    inline int ComposeCallTypes(int nObjectCallType, int nSystemCallType) {
        if (nObjectCallType == ctUseDefault)
            return GetDefaultCallType(nSystemCallType);

        int nCallTypeRestrictions = g_lMaskCallTypeRestriction & nSystemCallType;

        if (nObjectCallType == 0 ||  // Object call type does not defined
            (nCallTypeRestrictions == ctOnlyWindow) && (nObjectCallType & ctDocument) ||
            (nCallTypeRestrictions == ctOnlyDocument) && !(nObjectCallType & ctDocument)) {
            return GetDefaultCallType(nSystemCallType);
        } else
            return nCallTypeRestrictions | nObjectCallType;
    }

    inline LPCTSTR GetCallType(int nType) {
        switch (nType) {
        case ctAny:
            return _T("По выбору");
        case ctOnlyDocument:
            return _T("Документ");
        case ctOnlyWindow:
            return _T("Окно");
        case ctDocument:
            return _T("Документ");
        case ctWindow:
            return _T("Окно");
        case ctNewWindow:
            return _T("Новое окно");
        }
        return _T("");
    }
}  // namespace MS

////////////////////////////////////////////////////////
namespace CatID {
    enum { FirstListCat = 1000 };
    // Simple cats
    enum {
        Scans,
        General,
        Scale,
        Computer,
        Control,
        Imitation,
        Mnemo,
        Runtime,
        Schedule,
        Objects,
        Channels,
        SysConditions,
        Archives,
        ArchiveExport,
        ActionLog,
        MaxID,
        ExtAccess,
        ChannelsSettings  //Настройки каналов (индексы такие же как в Channels)
    };
    // List cats
    enum {
        AlarmCategories = FirstListCat,
        _MnemoTypes,  // Width, Height
        JournalTypes,
        _MnemoCallTypes,  // ECallType
        MnemoInfo,        // CLSID_MnemoInfo
        JournalInfo       // CLSID_MnemoInfo
    };
}  // namespace CatID

////////////////////////////////////////////////////////
namespace PropID {
    enum {
        ByTime,
        ByChange,
        InPercent,
        Period,
        Deadband,
        DeadbandPerc,
        WriteLog,
        HasCycle,
        Interleaving,
        MinPeriod,
        Balance,
        OpcScan,
        OpcPeriod,
        OpcDeadband,
        AlwaysEnable,
        CompPeriod,
        OpcPeriodFull,
        SaveRestart,
        OpcFullScanTimeout,
        OpcFullScanSync,
        OpcWriteByTime,
        SaveRestartPeriod,
        RestartLoadPeriod,
        UseRestartLoadPeriod,  // Scans
        UseOpcFullScan,
        OpcFullScanCondition,
        ScanCondition,
        ImpulsDuration,
        DisableTransferBackwardData,
        DisableTransferBackwardDataDuration,
        SetOPCCurrentTimeStamp,
        OpcReconnectCondition,
        PriorityParam,
        ControllingParam,
        Priority,
        OpcTimeValueUTC,
        OpcNotReadProps,
        ScanConditionEdge,
        ScanAggregateValueType,
        ScanAggregateObjectStateId,
        RTSetFBPoutQualityFromPins,
        RTWorkThreadCount,
        RTObjectCycleTimeout
    };
    enum {
        Comment,
        Type,
        PoutID,
        Imitation,
        RedefConst,
        Const,
        WriteArchive,
        StartupMnemoWindow,
        PreValue,
        UsePreValue,
        DisableInhType,
        AccurClass,
        CalibrMethod,
        LogicTreeVisible,
        UseDefaultName,
        ArchiveInterpolation,
        ExternalID,
        RedefinedPermissionComment,
        ShowInTrend,
        KKSValue,
        KKSFormatObject,
        EventMessagePrefix,
        TimeParamIndex,
        SetExtAccess,
        Description,
        IsTemplateObject,
        TemplateObjectID,
        TemplateObjectSystemInfo,
        HideInstanceContent,
        HideInstanceContentDefault,
        HideObjectContent,
        HideObjectContentInLibrary,
        HideObjectContentPassword,
        TemplateObjectRelativeId,
        HideObjectContentRTKeys,
        ShowInInternetClient,
        ShowInJournal,
        ShowInWindow,
        UseExternalLinksService,
        ExternalLinksServiceSettings,
        ObjectCallSystemSettings,
        ObjectCallSettings,
        StorePinInDbSettings,
        TemplateObjectsSettings,
        TemplateObjectsApplyFilter  // General
    };
    enum {
        Units,
        Interval,
        DiscrInterval,
        EventCategory,
        EventSeverity,
        EventChannels,
        EventAckRequired,
        EventSoundFile,
        EventMergeSoundCategory  //, Scale
    };
    enum {
        ID,
        ControllerID,
        DefaultDocumentKey,
        EnableCompReserve,
        StartupObjectID,
        ConnectionSettings  // Computer
    };
    enum {
        MinWarning,
        MaxWarning,
        MinError,
        MaxError,
        MaxSpeed,
        ControlDeadband  //, Control
    };
    enum {
        Rule,
        Min,
        Max,
        SinPeriod,
        SawPeriod,
        DiscrPeriod,
        IntPeriod,
        DiscrType,
        IntType  // Imitation
    };
    enum {
        ShowSplash,
        VVGraphOnly,
        FileName,
        PopupTitle,
        MainToolbar,
        CallToolbar,
        ShowObjectTree,
        ShowSystemTree,
        HideUnusedPages,
        LoadAllMnemo,
        LoadOptimizedMnemo,
        ShowStatusBar,
        ArchiveBlockSize,
        KeepInMemoryOnClose,
        OpenDocumentsAsWindow,
        CloseHiddenOnOpenWindow,
        NavigateCreatedJournals,
        TooltipSizeScale,
        RTLocaleId,
        RestrictRTKeysList,
        RestrictRTKeysEnable,
        RTActionsLimit,
        ProjectAuthorContacts,
        RestrictRTDuration,
        RestrictRTStopTime,
        StandardWinButtons,
        SaveMnemoRTState  // Mnemo
    };
    enum {
        Override,
        Save  // Runtime
    };
    enum {
        object,
        UseSystemActions  //, Schedule
    };
    enum {
        Parser,
        ChannelMask,
        OpcIgnoreBadValue,
        Passport,
        PermissionsObject,
        HotKey,
        MetrologySettings,
        ModemsSettings,
        ProjectSettings,
        OPCDiagnosticsConfig,
        DBConnectorSettings,
        DataArchiveSystemSettings,
        EventArchiveSystemSettings,
        HotRestartSettings,
        StartupWindowsSettings,
        ErrorReportSettings,
        ProjectExportSettings,
        ProjectImportSettings,
        JournalFiltersSettings,
        HotKeySequance,
        RightAssignmentsSettings,
        ProjectSaveInfo  //, Objects
    };
    enum {
        MaxSizeOld,
        DataTime,
        DataSize,
        AlarmsTime,
        AlarmSize,
        MnemoTime,
        MnemoSize,
        TrendTime,
        TrendSize,
        ReportTime,
        ReportSize,
        JournalTime,
        JournalSize,
        SysDataTime,
        SysDataSize,  // 14. 15-20 - reserved
        FreeSpaceReservedMb = 21,
        MaxSize = 22,
        ArchiveUseControlDeadband,
        ArchiveObjectTypesMask,
        ArchiveProviderId,
        ArchiveWriteMode,
        ArchiveDeadband,
        ArchiveTypeId,
        EventArchiveProviderId,
        EventsSpreadOnOtherComputers,
        ControllerArchiveId
    };                                                                                                 // Archives
    enum { DataExport, AlarmExport, ArchiveExportConnectorID, DbName, ArchiveExportUseRussianNames };  // ArchiveExport
    enum {
        OverrideActions,
        SaveActions  // ActionLog
    };
    enum { MaxAlarmCatID, MaxJournalID, MaxWorktimeID, MaxMnemoID };  // MaxID
    enum { ExtAccessMBAddress, ExtAccessDisableWrite };               // ExtAccess
}  // namespace PropID

////////////////////////////////////////////////////////
namespace Events {
    enum EventStatus { EventOn, EventOff, SimpleEvent };
    enum EventType { PoutEvent, FBEvent, ObjectEvent, SystemEvent, PassportEvent };

    enum SysCategories  // IDs of system categories
    {
        NotDefined,
        Error,
        Warning,
        UndefinedValue,
        Information,
        SystemError,
        ExecutiveMechanism
    };

    enum EConditionFlags {
        cfNotAcked = 0x1,
        cfSimpleEvent = 0x2,
        cfForComputers = 0x4,
        cfForOPC = 0x8,
        cfForController = 0x10,
        cfForExternalModule = 0x20,
        cfWithSubcondition = 0x40,
        cfForDBConnector = 0x80

    };

    enum ESysConditions {
        scDiskLowSpace,
        scLogin,
        scDateChanged,
        scComputerStartDisconnect,
        scComputerDisconnect,
        scComputerOtherDisconnect,
        scPrepareReport,
        scPrinterError,
        scOPCDisconnect,
        scOPCStatusFailed,
        scOPCStatusNoConfig,
        scOPCStatusSuspend,
        scOPCStatusTest,
        scOPCOverflow,
        scOPCWriteFailed,
        scControllerConfigLoading,
        scControllerDisconnect,
        scControllerConfigLoadingError,
        scControllerWritingError,
        scControllerInfoMessage,
        scRedundantControllerDisconnect,
        scRedundantControllerConfigLoading,
        scMainControllerDisconnect,
        scEstablishConnection,
        scOtherEstablishConnection,
        scEstablishConnectionError,
        scInternalModuleFault,
        scOtherInternalModuleFault,
        scExternalModuleFault,
        scOtherExternalModuleFault,
        scConnectionWithAnotherControllerFault,
        scOtherConnectionWithAnotherControllerFault,
        scConnectionWithUpperLevelFault,
        scOtherConnectionWithUpperLevelFault,
        scHandInhibitionOfControl,
        scOtherHandInhibitionOfControl,
        scModuleFault,
        scOtherModuleFault,
        scMainComputerFault,
        scRedundantComputerFault,
        scSwitchToMasterMode,
        scDBConnectorDataDisconnect,
        scDBConnectorDataError,
        scDBConnectorEventDisconnect,
        scDBConnectorEventError,
        scDBConnectorDataOverflow,
        scDBConnectorEventOverflow,
        scSystemError
    };

    struct CSysCondition {
        int nCondition;
        LPCTSTR cszEnName;
        LPCTSTR cszName;
        SysCategories categoryID;
        DWORD dwValidityMask;
        DWORD dwItemTypeMask;
        int nGroup;  // in AlarmManager  0 - OPC, 1 - range control, 2 - speed control
                     //(-FBEvent) - FB events, (-SystemEvent) - system events
        DWORD dwConditionFlags;

        EventType GetEventType() const {
            if (nGroup == -FBEvent)
                return FBEvent;
            else if (nGroup == -SystemEvent)
                return SystemEvent;
            else
                return PoutEvent;
        }
    };

    enum EAlarmConditionKey {
        // Pout conditions
        ackOPCDeviceFailure,
        ackOPCSensorFailure,
        ackControlMinError,
        ackControlMaxError,
        ackOPCConfigError,
        ackOPCNotConnected,
        ackOPCLastKnown,
        ackOPCCommFailure,
        ackControlMinWarning,
        ackControlMaxWarning,
        ackOPCOutOfService,
        ackControlMaxSpeed,
        // FB conditions
        ackFBFailure,
        ackFBChangeState,
        ackFBBlockControl,
        // System conditions
        ackDiskLowSpace,
        ackLogin,
        ackDateChanged,
        ackComputerStartDisconnect,
        ackComputerDisconnect,
        ackComputerOtherDisconnect,
        ackPrepareReport,
        ackPrinterError,
        // System conditions (OPC)
        ackOPCDisconnect,
        ackOPCStatusFailed,
        ackOPCStatusNoConfig,
        ackOPCStatusSuspend,
        ackOPCStatusTest,
        ackOPCOverflow,
        // FB events
        ackFBDivideByZero,
        ackFBPower,
        ackFBSinus,
        ackFBLn,
        ackFBAccuracy,
        ackFBInterval,
        ackFBClock,
        // Additional
        ackOPCWriteFailed,
        ackControllerConfigLoading,
        ackControllerDisconnect,
        ackControllerConfigLoadingError,
        ackControllerWritingError,
        ackControllerInfoMessage,
        ackRedundantControllerDisconnect,
        ackRedundantControllerConfigLoading,
        ackMainControllerDisconnect,
        ackEstablishConnection,
        ackOtherEstablishConnection,
        ackEstablishConnectionError,
        ackInternalModuleFault,
        ackOtherInternalModuleFault,
        ackExternalModuleFault,
        ackOtherExternalModuleFault,
        ackConnectionWithAnotherControllerFault,
        ackOtherConnectionWithAnotherControllerFault,
        ackConnectionWithUpperLevelFault,
        ackOtherConnectionWithUpperLevelFault,
        ackHandInhibitionOfControl,
        ackOtherHandInhibitionOfControl,
        ackModuleFault,
        ackOtherModuleFault,
        ackMainComputerFault,
        ackRedundantComputerFault,
        ackSwitchToMasterMode,
        ackDBConnectorDataDisconnect,
        ackDBConnectorDataError,
        ackDBConnectorEventDisconnect,
        ackDBConnectorEventError,
        ackDBConnectorDataOverflow,
        ackDBConnectorEventOverflow,
        ackOPCBad,
        ackOPCUncertain,
        ackSystemError
    };

    // Default settings for CatID::SysConditions
    const CSysCondition SysConditions[] = {
        {-1,
         _T("Device failure"),
         _T("Ошибка устройства"),
         UndefinedValue,
         OPC_QUALITY_DEVICE_FAILURE,
         MS::MASK(MS::TIT_POUT),
         0,
         0},
        {-1,
         _T("Sensor failure"),
         _T("Ошибка датчика"),
         UndefinedValue,
         OPC_QUALITY_SENSOR_FAILURE,
         MS::MASK(MS::TIT_POUT),
         0,
         0},
        {-1, _T("LowLow limit"), _T("Минимальное аварийное"), Error, MS::PC_MIN_ERROR, MS::MASK(MS::TIT_POUT), 1, 0},
        {-1, _T("HighHigh limit"), _T("Максимальное аварийное"), Error, MS::PC_MAX_ERROR, MS::MASK(MS::TIT_POUT), 1, 0},
        {-1,
         _T("Config error"),
         _T("Ошибка конфигурации"),
         Warning,
         OPC_QUALITY_CONFIG_ERROR,
         MS::MASK(MS::TIT_POUT),
         0,
         0},
        {-1,
         _T("Not connected"),
         _T("Нет соединения"),
         UndefinedValue,
         OPC_QUALITY_NOT_CONNECTED,
         MS::MASK(MS::TIT_POUT),
         0,
         0},
        {-1,
         _T("Last known"),
         _T("Последнее известное значение"),
         Information,
         OPC_QUALITY_LAST_KNOWN,
         MS::MASK(MS::TIT_POUT),
         0,
         cfNotAcked},
        {-1,
         _T("Comm failure"),
         _T("Нет связи"),
         UndefinedValue,
         OPC_QUALITY_COMM_FAILURE,
         MS::MASK(MS::TIT_POUT),
         0,
         0},
        {-1,
         _T("Low limit"),
         _T("Минимальное предупреждающее"),
         Warning,
         MS::PC_MIN_WARNING,
         MS::MASK(MS::TIT_POUT),
         1,
         0},
        {-1,
         _T("High limit"),
         _T("Максимальное предупреждающее"),
         Warning,
         MS::PC_MAX_WARNING,
         MS::MASK(MS::TIT_POUT),
         1,
         0},
        {-1,
         _T("Out of service"),
         _T("Не обслуживается"),
         UndefinedValue,
         OPC_QUALITY_OUT_OF_SERVICE,
         MS::MASK(MS::TIT_POUT),
         0,
         0},
        {-1,
         _T("High rate of change"),
         _T("Максимальная скорость изменения"),
         Error,
         MS::PC_MAX_SPEED,
         MS::MASK(MS::TIT_POUT),
         2,
         0},
        {-1,
         _T("Executive mechanism error"),
         _T("Неисправность исполнительного механизма"),
         Error,
         0,
         MS::MASK(MS::TIT_OBJECT),
         -FBEvent,
         0},
        {-1,
         _T("Executive mechanism change state"),
         _T("Изменение режима исполнительного механизма"),
         ExecutiveMechanism,
         0,
         MS::MASK(MS::TIT_OBJECT),
         -FBEvent,
         cfNotAcked},
        {-1,
         _T("Executive mechanism block control"),
         _T("Блокировка управления исполнительным механизмом"),
         ExecutiveMechanism,
         0,
         MS::MASK(MS::TIT_OBJECT),
         -FBEvent,
         cfNotAcked},
        // System events
        {scDiskLowSpace, _T("Disk low space"), _T("На диске осталось мало места"), Warning, 0, 0, -SystemEvent, 0},
        {scLogin, _T("Login"), _T("Вход в систему"), Information, 0, 0, -SystemEvent, cfNotAcked},
        {scDateChanged,
         _T("Date changed"),
         _T("Смена даты"),
         Information,
         0,
         0,
         -SystemEvent,
         cfSimpleEvent | cfNotAcked},
        {scComputerStartDisconnect,
         _T("No connection with computer"),
         _T("Связь с компьютером не установлена"),
         Warning,
         0,
         0,
         -SystemEvent,
         cfForComputers | cfWithSubcondition},
        {scComputerDisconnect,
         _T("Connection broken"),
         _T("Обрыв связи с компьютером"),
         UndefinedValue,
         0,
         0,
         -SystemEvent,
         cfForComputers | cfWithSubcondition},
        {scComputerOtherDisconnect,
         _T("Computer disconnected"),
         _T("Отключение компьютера"),
         Information,
         0,
         0,
         -SystemEvent,
         cfForComputers | cfNotAcked | cfWithSubcondition},
        {scPrepareReport,
         _T("Report creating"),
         _T("Формирование рапорта"),
         Information,
         0,
         0,
         -SystemEvent,
         cfNotAcked},
        {scPrinterError, _T("Printer error"), _T("Ошибка принтера"), Warning, 0, 0, -SystemEvent, 0},
        {scOPCDisconnect,
         _T("Broken connection with OPC"),
         _T("Нарушена связь с OPC сервером"),
         UndefinedValue,
         0,
         0,
         -SystemEvent,
         cfForOPC},
        {scOPCStatusFailed, _T("OPC error"), _T("Ошибка в OPC сервере"), UndefinedValue, 0, 0, -SystemEvent, cfForOPC},
        {scOPCStatusNoConfig,
         _T("OPC don't have config"),
         _T("Конфигурация OPC сервера не загружена"),
         Warning,
         0,
         0,
         -SystemEvent,
         cfForOPC},
        {scOPCStatusSuspend,
         _T("OPC suspended"),
         _T("OPC сервер приостановлен"),
         Warning,
         0,
         0,
         -SystemEvent,
         cfForOPC},
        {scOPCStatusTest,
         _T("OPC in test mode"),
         _T("OPC сервер функционирует в тестовом режиме"),
         Information,
         0,
         0,
         -SystemEvent,
         cfForOPC | cfNotAcked},
        {scOPCOverflow,
         _T("OPC overflow"),
         _T("Система не успевает обработать данные от OPC сервера"),
         Warning,
         0,
         0,
         -SystemEvent,
         cfForOPC},
        // FB events
        {-1, _T("Divide by zero"), _T("Деление на 0"), SystemError, 0, MS::MASK(MS::TIT_OBJECT), -FBEvent, cfNotAcked},
        {-1,
         _T("Invalid power"),
         _T("Недопустимое основание степени"),
         SystemError,
         0,
         MS::MASK(MS::TIT_OBJECT),
         -FBEvent,
         cfNotAcked},
        {-1,
         _T("Invalid period"),
         _T("Недопустимый период тригонометрической функции"),
         SystemError,
         0,
         MS::MASK(MS::TIT_OBJECT),
         -FBEvent,
         cfNotAcked},
        {-1,
         _T("Invalid logarithm base"),
         _T("Недопустимое основание логарифма"),
         SystemError,
         0,
         MS::MASK(MS::TIT_OBJECT),
         -FBEvent,
         cfNotAcked},
        {-1,
         _T("Invalid accuracy"),
         _T("Некорректная точность сравнения"),
         SystemError,
         0,
         MS::MASK(MS::TIT_OBJECT),
         -FBEvent,
         cfNotAcked},
        {-1,
         _T("Invalid interval"),
         _T("Некорректный интервал времени"),
         SystemError,
         0,
         MS::MASK(MS::TIT_OBJECT),
         -FBEvent,
         cfNotAcked},
        {-1,
         _T("Invalid time"),
         _T("Некорректное значение времени"),
         SystemError,
         0,
         MS::MASK(MS::TIT_OBJECT),
         -FBEvent,
         cfNotAcked},
        // Additional
        {scOPCWriteFailed,
         _T("Write to OPC failed"),
         _T("Ошибка записи в переменную OPC сервера"),
         Warning,
         0,
         0,
         -SystemEvent,
         cfSimpleEvent},
        // Controllers
        {scControllerConfigLoading,
         _T("Controller load config"),
         _T("Загрузка конфигурации контроллера"),
         Information,
         0,
         0,
         -SystemEvent,
         cfForController | cfNotAcked | cfWithSubcondition},
        {scControllerConfigLoadingError,
         _T("Error while loading config to controller"),
         _T("Ошибка загрузки конфигурации контроллера"),
         Error,
         0,
         0,
         -SystemEvent,
         cfForController | cfSimpleEvent | cfWithSubcondition},
        {scControllerDisconnect,
         _T("Controller disconnected"),
         _T("Нет соединения с контроллером"),
         Error,
         0,
         0,
         -SystemEvent,
         cfForController | cfWithSubcondition},
        {scControllerWritingError,
         _T("Write to controller failed"),
         _T("Ошибка записи данных в контроллер"),
         Error,
         0,
         0,
         -SystemEvent,
         cfForController | cfSimpleEvent | cfWithSubcondition},
        {scControllerInfoMessage,
         _T("Message from controller"),
         _T("Сообщение от контроллера"),
         Information,
         0,
         0,
         -SystemEvent,
         cfForController | cfSimpleEvent},
        {scRedundantControllerConfigLoading,
         _T("(controller 2) Controller load config"),
         _T("(контроллер 2) Загрузка конфигурации контроллера"),
         Information,
         0,
         0,
         -SystemEvent,
         cfForController | cfNotAcked | cfWithSubcondition},
        {scRedundantControllerDisconnect,
         _T("(controller 2) Controller disconnected"),
         _T("(контроллер 2) Нет соединения с контроллером"),
         Warning,
         0,
         0,
         -SystemEvent,
         cfForController | cfWithSubcondition},
        {scMainControllerDisconnect,
         _T("(controller 1) Controller disconnected"),
         _T("(контроллер 1) Нет соединения с контроллером"),
         Warning,
         0,
         0,
         -SystemEvent,
         cfForController | cfWithSubcondition},
        {scEstablishConnection,
         _T("Controller connected"),
         _T("Установлено соединение"),
         Information,
         0,
         0,
         -SystemEvent,
         cfForController | cfNotAcked | cfWithSubcondition},
        {scOtherEstablishConnection,
         _T("(controller 2) Controller connected"),
         _T("(контроллер 2) Установлено соединение"),
         Information,
         0,
         0,
         -SystemEvent,
         cfForController | cfNotAcked | cfWithSubcondition},
        {scEstablishConnectionError,
         _T("Controller connection failed"),
         _T("Ошибка установки соединения"),
         Error,
         0,
         0,
         -SystemEvent,
         cfForController | cfWithSubcondition},
        {scInternalModuleFault,
         _T("Internal modules failure"),
         _T("Отказ встроенных модулей УСО"),
         Error,
         0,
         0,
         -SystemEvent,
         cfForController},
        {scOtherInternalModuleFault,
         _T("(controller 2) Internal modules failure"),
         _T("(контроллер 2) Отказ встроенных модулей УСО"),
         Error,
         0,
         0,
         -SystemEvent,
         cfForController},
        {scExternalModuleFault,
         _T("External modules failure"),
         _T("Отказ внешних модулей УСО"),
         Error,
         0,
         0,
         -SystemEvent,
         cfForController},
        {scOtherExternalModuleFault,
         _T("(controller 2) External modules failure"),
         _T("(контроллер 2) Отказ внешних модулей УСО"),
         Error,
         0,
         0,
         -SystemEvent,
         cfForController},
        {scConnectionWithAnotherControllerFault,
         _T("Connection with other controller failed"),
         _T("Отказ связи с другим контроллером"),
         Error,
         0,
         0,
         -SystemEvent,
         cfForController},
        {scOtherConnectionWithAnotherControllerFault,
         _T("(controller 2) Connection with other controller failed"),
         _T("(контроллер 2) Отказ связи с другим контроллером"),
         Error,
         0,
         0,
         -SystemEvent,
         cfForController},
        {scConnectionWithUpperLevelFault,
         _T("Connection with upper level failed"),
         _T("Отказ связи с верхним уровнем"),
         Error,
         0,
         0,
         -SystemEvent,
         cfForController},
        {scOtherConnectionWithUpperLevelFault,
         _T("(controller 2) Connection with upper level failed"),
         _T("(контроллер 2) Отказ связи с верхним уровнем"),
         Error,
         0,
         0,
         -SystemEvent,
         cfForController},
        {scHandInhibitionOfControl,
         _T("Manual block control"),
         _T("Ручной запрет управления"),
         Error,
         0,
         0,
         -SystemEvent,
         cfForController},
        {scOtherHandInhibitionOfControl,
         _T("(controller 2) Manual block control"),
         _T("(контроллер 2) Ручной запрет управления"),
         Error,
         0,
         0,
         -SystemEvent,
         cfForController},
        {scModuleFault, _T("Module failure"), _T("Отказ модуля"), Error, 0, 0, -SystemEvent, cfForExternalModule},
        {scOtherModuleFault,
         _T("(controller 2) Module failure"),
         _T("(контроллер 2) Отказ модуля"),
         Error,
         0,
         0,
         -SystemEvent,
         cfForExternalModule},
        {scMainComputerFault,
         _T("Main controller failure"),
         _T("Отказ основного"),
         Error,
         0,
         0,
         -SystemEvent,
         cfForComputers},
        {scRedundantComputerFault,
         _T("Redundant controller failure"),
         _T("Отказ резервного"),
         Error,
         0,
         0,
         -SystemEvent,
         cfForComputers},
        {scSwitchToMasterMode,
         _T("Computer change master state"),
         _T("Переключение режима работы компьютера"),
         Information,
         0,
         0,
         -SystemEvent,
         cfForComputers},
        {scDBConnectorDataDisconnect,
         _T("Data archive: no connection"),
         _T("Архивирование данных: нет связи с БД"),
         SystemError,
         0,
         0,
         -SystemEvent,
         cfForDBConnector},
        {scDBConnectorDataError,
         _T("Data archive error:"),
         _T("Архивирование данных - ошибка:"),
         Error,
         0,
         0,
         -SystemEvent,
         cfForDBConnector | cfSimpleEvent},
        {scDBConnectorEventDisconnect,
         _T("Event archive: no connection"),
         _T("Архивирование сообщений: нет связи с БД"),
         SystemError,
         0,
         0,
         -SystemEvent,
         cfForDBConnector},
        {scDBConnectorEventError,
         _T("Event archive error:"),
         _T("Архивирование сообщений - ошибка:"),
         Error,
         0,
         0,
         -SystemEvent,
         cfForDBConnector | cfSimpleEvent},
        {scDBConnectorDataOverflow,
         _T("Data archive: DB overflow"),
         _T("Архивирование данных: переполнение БД"),
         SystemError,
         0,
         0,
         -SystemEvent,
         cfForDBConnector},
        {scDBConnectorEventOverflow,
         _T("Event archive: DB overflow"),
         _T("Архивирование сообщений: переполнение БД"),
         SystemError,
         0,
         0,
         -SystemEvent,
         cfForDBConnector},
        // OPC
        {-1, _T("Error"), _T("Ошибка"), UndefinedValue, OPC_QUALITY_BAD, MS::MASK(MS::TIT_POUT), 0, 0},
        {-1, _T("Uncertain"), _T("Не определено"), UndefinedValue, OPC_QUALITY_UNCERTAIN, MS::MASK(MS::TIT_POUT), 0, 0},
        {scSystemError, _T(""), _T("Системная ошибка"), SystemError, 0, 0, -SystemEvent, 0},
        {-1, NULL, NULL, NotDefined, 0, 0, 0}};

    enum ChannelIndex {
        ChannelPopup = 0,
        ChannelStatus = 1,
        ChannelJournal = 2,
        ChannelPrinter = 3,
        ChannelArchive = 4,
        ChannelEMail = 5,
        ChannelSMS = 6,
        ChannelSound = 7,
        ChannelsCount = 8
    };

    const LPCTSTR g_szChannelNames[] = {_T("Сообщение"),
                                        _T("Строка статуса"),
                                        _T("Журнал"),
                                        _T("Принтер"),
                                        _T("Архив"),
                                        _T("E-mail"),
                                        _T("СМС"),
                                        _T("Звук")};

    const DWORD CHANNEL_POPUP = 0x00000001;
    const DWORD CHANNEL_STATUS = 0x00000002;
    const DWORD CHANNEL_JOURNAL = 0x00000004;
    const DWORD CHANNEL_PRINTER = 0x00000008;
    const DWORD CHANNEL_ARCHIVE = 0x00000010;

}  // namespace Events

namespace MS {
    enum EDocTypeSource { dtsTyped, dtsNamed, dtsOtherDocs };

    enum EDocTypeInfoFlags { dtifGlobalAccess = 1, dtifShowPalette = 2 };

    struct CDocTypeInfo {
        EDocTypeSource Source;
        LPCTSTR szName;
        int nListCat;
        LPCTSTR szPrefix;
        LPCTSTR szActions;  //_T("Открыть\nЗакрыть\nПросмотр\nПечать\nСохранить\nПрисвоить\nЭкспорт")
        LPCTSTR szFileExt;
        DWORD dwFlags;
        int nArchiveSettings;
    };

    //#if defined(REPORT_VERSION)
    // const CDocTypeInfo g_rgDocTypes[] = {
    //	{dtsTyped, _T("Окна"), CatID::MnemoInfo, _T("M"), _T("Открыть\nЗакрыть\nПросмотр\nПечать\nСохранить\nОткрыть
    //сохраненные"), 		_T(""), dtifGlobalAccess, PropID::MnemoTime}, 	{dtsNamed, _T("Отчеты"), 0, _T("R"),
    //_T("Просмотр\nПечать\nСохранить\nОткрыть сохраненные"), 		_T(".xls"), dtifGlobalAccess, PropID::ReportTime}
    //};
    //#else
    const CDocTypeInfo g_rgDocTypes[] = {{dtsTyped,
                                          _T("Окна"),
                                          CatID::MnemoInfo,
                                          _T("M"),
                                          _T("Открыть\nЗакрыть\nПросмотр\nПечать\nСохранить\nОткрыть сохраненные"),
                                          _T(""),
                                          dtifGlobalAccess,
                                          PropID::MnemoTime},
                                         {dtsTyped,
                                          _T("Журналы"),
                                          CatID::JournalTypes,
                                          _T("J"),
                                          _T("Открыть\nЗакрыть\nПечать\nОткрыть сохраненные"),
                                          _T(".jrn"),
                                          dtifGlobalAccess,
                                          PropID::JournalTime},
                                         {dtsNamed,
                                          _T("Тренды"),
                                          0,
                                          _T("T"),
                                          _T("Открыть\nЗакрыть\nПросмотр\nПечать\nСохранить\nОткрыть сохраненные"),
                                          _T(".mtc"),
                                          dtifGlobalAccess,
                                          PropID::TrendTime},
                                         {dtsNamed,
                                          _T("Отчеты"),
                                          0,
                                          _T("R"),
                                          _T("Просмотр\nЗакрыть\nПечать\nСохранить\nОткрыть сохраненные"),
                                          _T(".xls"),
                                          dtifGlobalAccess,
                                          PropID::ReportTime},
                                         {dtsOtherDocs,
                                          _T("Другие"),
                                          0,
                                          _T("O"),
                                          _T("Открыть\nЗакрыть\nПросмотр\nПечать\nСохранить"),
                                          _T(""),
                                          dtifGlobalAccess,
                                          -1},
                                         {dtsNamed,
                                          _T("Схема"),
                                          0,
                                          _T("L"),
                                          _T("Открыть\nЗакрыть\nПросмотр\nПечать\nСохранить"),
                                          _T(""),
                                          dtifShowPalette,
                                          -1}};
    //#endif

    const int g_nDocTypesCount = sizeof(g_rgDocTypes) / sizeof(g_rgDocTypes[0]);

    const DWORD g_dwMnemoImage = 100L;  // ID of the document type "Mnemo image"

    const TCHAR g_szRegMainKey[] = _T("SOFTWARE\\InSAT Company\\MasterSCADA");
    const TCHAR g_szRegPathValue[] = _T("MainPath");
    const TCHAR g_szRegLastLogTimeValue[] = _T("LastLogTime");
    const TCHAR g_szRegRunningValue[] = _T("Running");

    const TCHAR g_szPathForShells[] = _T("\\Commands");
    const TCHAR g_szInvalidFileChars[] = _T("<>:/\\|?*");

#ifdef _ABV
    const TCHAR g_szRegKeyWithMainPath[] = _T("Software\\InSAT Company\\Master SCADA ABV");
#elif defined(_TREND)
    const TCHAR g_szRegKeyWithMainPath[] = _T("Software\\InSAT Company\\MasterTrend");
#elif defined(_LOGIC)
    const TCHAR g_szRegKeyWithMainPath[] = _T("Software\\InSAT Company\\MasterLogic");
#elif defined(REPORT_VERSION)
    const TCHAR g_szRegKeyWithMainPath[] = _T("Software\\InSAT Company\\MasterReport");
#else
    const TCHAR g_szRegKeyWithMainPath[] = _T("SOFTWARE\\InSAT Company\\MasterSCADA");
#endif

    inline bool IsAnalogType(VARTYPE vt) {
        return vt == VT_I4 || vt == VT_UI4 || vt == VT_R4 || vt == VT_R8 || vt == VT_I2 || vt == VT_UI2 ||
               vt == VT_INT || vt == VT_UINT || vt == VT_I1 || vt == VT_UI1 || vt == VT_DECIMAL;
    }

    inline DATE FileTimeToVariantTime(const FILETIME* pft) {
        SYSTEMTIME st;
        if (!::FileTimeToSystemTime(pft, &st))
            return 0.0;

        SYSTEMTIME st_local;
        if (!::SystemTimeToTzSpecificLocalTime(NULL, &st, &st_local))
            return 0.0;

        WORD wMilliseconds = st_local.wMilliseconds;
        st_local.wMilliseconds = 0;
        double dblTime;
        if (!::SystemTimeToVariantTime(&st_local, &dblTime))
            return 0.0;
        dblTime += ((double)wMilliseconds) / ((double)24 * 60 * 60 * 1000);

        return dblTime;
    }

    inline bool VariantTimeToFileTime(DATE dateTime, FILETIME* pft) {
        SYSTEMTIME st_local;
        if (!::VariantTimeToSystemTime(dateTime, &st_local))
            return false;
        double dblTimeWithoutMS;
        if (!::SystemTimeToVariantTime(&st_local, &dblTimeWithoutMS))
            return false;

        SYSTEMTIME st;
        if (!::TzSpecificLocalTimeToSystemTime(NULL, &st_local, &st))
            return false;
        if (!::SystemTimeToFileTime(&st, pft))
            return false;

        int diffMS = (int)((dateTime - dblTimeWithoutMS) * (double)24 * 60 * 60 * 1000);
        *((ULONGLONG*)pft) += ((ULONGLONG)diffMS) * g_lFileTimeMiliseconds;
        return true;
    }

#ifdef __ATLCOMTIME_H__
    inline COleDateTime GetCurrentDateTime() {
        SYSTEMTIME st;
        GetLocalTime(&st);

        WORD wMilliseconds = st.wMilliseconds;
        st.wMilliseconds = 0;
        double dblTime;
        if (!::SystemTimeToVariantTime(&st, &dblTime))
            return COleDateTime();
        dblTime += ((double)wMilliseconds) / ((double)24 * 60 * 60 * 1000);

        return dblTime;
    }

    inline void SetCurrentDate(COleDateTime& odtTime) {
        COleDateTime odtCurData = GetCurrentDateTime();
        if (long(odtTime.m_dt) != long(odtCurData.m_dt))
            odtTime.m_dt += long(odtCurData.m_dt) - long(odtTime.m_dt);

#    ifdef _DEBUG
        CString str(odtTime.Format());
#    endif
    }
#endif

}  // namespace MS

#ifdef __ATLCOMTIME_H__
inline COleDateTime FileTimeToOle(const FILETIME& filetimeSrc) {
    // Momotov 19.02.2002
    //Добавил преобразование в локальное время
    COleDateTime res(MS::FileTimeToVariantTime(&filetimeSrc));
    res.m_status = res.m_dt != 0.0 ? COleDateTime::valid : COleDateTime::invalid;
    return res;
}
#endif

#endif
