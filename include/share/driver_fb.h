#pragma once

#include <share/lua_data_provider.h>
#include "share/base_fb.h"
#include "core/drivers/drv_user.h"


struct DriverVarDsc {
    enum {
        Input = 0,   // write to lua
        Output = 1,  // read from lua
        InOut = 2
    };
    BYTE Direction;  // Направление передачи. 0 - входная переменная (читаем из драйвера), 1 - выходная переменная
                     // (пишем в драйвер), 2 - вход/выход
    OpcUa_VariantHlp Value;
    int64_t Id;
    std::string Path;
    int64_t WriteId;
    std::string WritePath;
    OpcUa_BuiltInTypeHlp ReadType;
    OpcUa_BuiltInTypeHlp WriteType;

    bool Changed;      // Признак того что переменная изменена по подписке
    bool NeedToWrite;  // Признак того что переменная должна быть отправлена независимо от того изменилось или нет
                       // значение
    OpcUa_VariantHlp ValueToWrite;  //Значение для записи
    int64_t ValueToWriteTime;       //Значение для записи
    DriverVarDsc() {
        Direction = 0;
        Id = 0;
        WriteId = 0;
        Value.ArrayType = 0;
        Value.Datatype = OpcUaType_Null;
        ValueToWrite.ArrayType = 0;
        ValueToWrite.Datatype = OpcUaType_Null;
        NeedToWrite = false;
        Changed = false;
        ValueToWriteTime = 0;
    }
    DriverVarDsc(int64_t read_id, const char* readVarPath, int64_t write_id, const char* writeVarPath) {
        ValueToWriteTime = 0;

        // both can't be zero
        Direction = write_id ? read_id ? InOut : Output : Input;
        Id = read_id;
        if (readVarPath != NULL)
            Path = readVarPath;
        WriteId = write_id;
        if (writeVarPath != NULL)
            WritePath = writeVarPath;

        Value.ArrayType = 0;
        Value.Datatype = OpcUaType_Null;
        ValueToWrite.ArrayType = 0;
        ValueToWrite.Datatype = OpcUaType_Null;
        NeedToWrite = false;
        Changed = false;
    }

    void SetReadValue(WriteDataRec* rec, const OpcUa_DataValue& value) {
        if (ReadType.Type == OpcUaType_DataValue) {
            rec->_value.SetDataValue(value);
            if (value.Value.Datatype == OpcUaType_Null && Value.Datatype == OpcUaType_DataValue &&
                Value.Value.DataValue->Value.Datatype != OpcUaType_Null)  //Replace null value with old non null
                OpcUa_VariantHlp::ConvAndCopy(Value.Value.DataValue->Value,
                                              rec->_value.Value.DataValue->Value,
                                              ReadType.SubType);
            else if (ReadType.SubType != value.Value.Datatype && ReadType.SubType != OpcUaType_Null)
                OpcUa_VariantHlp::ConvAndCopy(value.Value, rec->_value.Value.DataValue->Value, ReadType.SubType);
        } else {
            rec->_valueType = ReadType;
            if (value.Value.Datatype == OpcUaType_Null && Value.Datatype != OpcUaType_Null)  // Replace null value with old non null
                OpcUa_VariantHlp::ConvAndCopy(Value, rec->_value, ReadType.Type);
            else if (ReadType.Type != value.Value.Datatype && ReadType.Type != OpcUaType_Null)
                OpcUa_VariantHlp::ConvAndCopy(value.Value, rec->_value, ReadType.Type);
            else
                rec->_value.CopyFrom(value.Value);
        }
        Value.CopyFrom(rec->_value);  // Записываем значение переменной в контейнер переменных драйвера
    }
};

typedef std::map<std::string, OpcUa_VariantHlp> MapStringToVariant;

inline int GetIntFieldFromMap(const MapStringToVariant& m, const std::string& key, int defaultValue = 0) {
    MapStringToVariant::const_iterator it = m.find(key);
    if (it == m.end())
        return defaultValue;

    int i = defaultValue;
    it->second.GetInt(i);
    return i;
}

inline bool GetBoolFieldFromMap(const MapStringToVariant& m, const std::string& key, bool defaultValue = false) {
    MapStringToVariant::const_iterator it = m.find(key);
    if (it == m.end())
        return defaultValue;

    bool b = defaultValue;
    it->second.GetBool(b);
    return b;
}

inline std::string GetStringFieldFromMap(const MapStringToVariant& m,
                                         const std::string& key,
                                         const std::string defaultValue = std::string()) {
    MapStringToVariant::const_iterator it = m.find(key);
    if (it == m.end())
        return defaultValue;

    std::string res;
    it->second.GetString(res);
    return res;
}

// Уникальный идентификатор класса драйвера. необходим для трансляции кодов состояний переменных в/из StatusCode OPC UA
// Может принимать значения от 0x00000000 до 0x3F000000
enum DriverTypeID {
    DriverType_OPC_UA_Client = 0x00000000, 
    DriverType_IEC_61850_Client = 0x01000000, 
    DriverType_IEC_104_Client = 0x02000000
};

template<class Driver>
class DriverFB : public BaseFB<Driver> {
protected:
    // Начальный интервал времени переподключения (сек.)
    static const unsigned int _BeginReconnectInterval = 5;
    // Максимальное интервал времени переподключения (с)
    static const unsigned int _MaxReconnectInterval = 60;

    enum DriverStates { NotConnected = 0, Connected = 1, Inited = 2,  Connecting = 4};

    DriverStates _DriverState;

    LuaDataProvider* _DataProvider;
    // Признак того что в LuaDataProvider можно загружать данные о ВМ
    bool _DPInit;
    int64_t _FaultItemId;
    int64_t _ErrorTextItemId;
    int64_t _ExecuteItemId;
    int64_t _ConnectItemId;
    int64_t _WriteItemId;
    enum ProtocolWriteCondition _writeCondition;
    bool _executeOnSlave;
    bool _writeOnSlave;

    bool _lastFault;    // состояние драйвера. true - ошибка false - нормально.
    bool _lastExecute;
    bool _lastWrite;

    time_t _LastTimeToConnect;
    // Интервал в секундах для попыток переподключения если сесия была разорвана
    unsigned int _ReconnectInterval;
    // Кол-во попыток переподключения если сесия была разорвана
    unsigned int _ReconnectNum;
    // Счетчик попыток переподключения если сесия была разорвана
    unsigned int _ReconnectCount;

    // Идентификатор драйвера. Может принимать значения от 0 до 63
    char _UniqueDriverID;

    virtual void AddChannel(int64_t /*readVarID*/,
                            const char* /* readVarPath */,
                            int64_t /* writeVarID */,
                            const char* /* writeVarPath */,
                            const MapStringToVariant& /* fields */) {}
    virtual void CalcRecconectInterval(void);
    void UpdateFaultState(bool fault, const std::string& errorText = "");
    void SetErrorText(const std::string& errorText );
    bool isExecute();
    bool isConnect() const;
    bool isWrite();
    bool isWriteEnabled();
    bool IsNeedWriteValue(const OpcUa_VariantHlp& value, DriverVarDsc& dsc, bool allowWriteBadValues = false, bool writeIfNotChanged = false);
    void InitVar(DriverVarDsc& dsc);

    virtual void InitedInternal(lua_State*) {
        //_DataProvider = LuaDataProvider::AddLuaDataProvider(L);
    }

    int setFieldCommon(const char* key, lua_State* L);

public:
    DriverFB(lua_State* L, const std::string& name);
    virtual int setField(lua_State* L);
    virtual int call(lua_State* L);
};

template<typename Driver>
DriverFB<Driver>::DriverFB(lua_State* L, const std::string& name)
    : BaseFB<Driver>(L, name), _DriverState(NotConnected) {
    _DPInit = false;
    _FaultItemId = 0;
    _lastFault = false;
    _LastTimeToConnect = 0;
    _ReconnectInterval = 5;
    _ReconnectNum = 5;
    _ReconnectCount = 0;
    _ErrorTextItemId = 0;
    _ExecuteItemId = 0;
    _lastExecute = false;
    _lastWrite = false;
    _ConnectItemId = 0;
    _WriteItemId = 0;
    _writeCondition = pwcByChange;
    _executeOnSlave = false;
    _writeOnSlave = false;
    _DataProvider = LuaDataProvider::AddLuaDataProvider(L);
    _UniqueDriverID = 0;
}

template<class Driver>
bool DriverFB<Driver>::isExecute() {
    if (_ExecuteItemId == 0)
        return false;
    OpcUa_VariantHlp value;
    _DataProvider->ReadValue(_ExecuteItemId, 0, std::string(), OpcUaType_Boolean, &value);
    bool bValue = false;
    value.GetBool(bValue);
    bool res = bValue && !_lastExecute;
    _lastExecute = bValue;
    return res;
}

template<class Driver>
bool DriverFB<Driver>::isConnect() const {
    if (!_executeOnSlave && IsSystemInReserv())
        return false;
    if (IsDisableRemotePorts())
        return false;
    if (_ConnectItemId == 0)
        return true;
    OpcUa_VariantHlp value;
    _DataProvider->ReadValue(_ConnectItemId, 0, std::string(), OpcUaType_Boolean, &value);
    bool res = false;
    value.GetBool(res);
    return res;
}

///
/// возвращает признак того, что переменная драйвера
/// 1. Должна быть записана. Переменная драйвера, содержащия признак записи по условию не равна 0 или или переменная с
/// признаком записи по условию отсутствует.
/// 2. Может быть записана. Драйвер выполняется либо на одиночном контроллере (без резервирования либо, для системы с
/// резервированием драйвер выполняется или на основном контроллере
///	или на резервном с включеннами признаками выполнения драйвера на резервном и выполнении записи на резервном.
///
template<class Driver>
bool DriverFB<Driver>::isWrite() {
    if (!isWriteEnabled())
        return false;
    if (_WriteItemId == 0)
        return true;
    OpcUa_VariantHlp value;
    _DataProvider->ReadValue(_WriteItemId, 0, std::string(), OpcUaType_Boolean, &value);
    bool bValue = false;
    value.GetBool(bValue);
    bool res = bValue && !_lastWrite;
    _lastWrite = bValue;
    return res;
}

template<class Driver>
bool DriverFB<Driver>::isWriteEnabled() {
    if ((!_executeOnSlave || !_writeOnSlave) && IsSystemInReserv())
        return false;
    if (IsDisableRemotePorts())
        return false;
    return true;
}

/*
* Разбор и инициализация настроек общая для всех драйверов на классе DriverFB.
* возвращает
* 0 - параметр не
 * разобран. требуется индивидуальная разборка настройки
* 1 - Настройка
 * получена.
*/
template<class Driver>
int DriverFB<Driver>::setFieldCommon(const char* key, lua_State* L) {
    if (strcmp(key, "FaultItemId") == 0) {
        _FaultItemId = lua_tointeger(L, -1);
        return 1;
    }
    if (strcmp(key, "ErrorTextItemId") == 0) {
        _ErrorTextItemId = lua_tointeger(L, -1);
        return 1;
    }
    if (strcmp(key, "ExecuteItemId") == 0) {
        _ExecuteItemId = lua_tointeger(L, -1);
        return 1;
    }
    if (strcmp(key, "ConnectItemId") == 0) {
        _ConnectItemId = lua_tointeger(L, -1);
        return 1;
    }
    if (strcmp(key, "WriteItemId") == 0) {
        _WriteItemId = lua_tointeger(L, -1);
        return 1;
    }
    if (strcmp(key, "WriteCondition") == 0) {
        _writeCondition = (ProtocolWriteCondition)lua_tointeger(L, -1);
        return 1;
    }
    if (strcmp(key, "ExecuteOnSlave") == 0) {
        _executeOnSlave = (bool)lua_toboolean(L, -1);
        return 1;
    }
    if (strcmp(key, "WriteOnSlave") == 0) {
        _writeOnSlave = (bool)lua_toboolean(L, -1);
        return 1;
    }
    return 0;
}


template<typename Driver>
int DriverFB<Driver>::setField(lua_State* L) {
    const char* key = lua_tostring(L, -2);
    if (setFieldCommon(key, L) != 0)
        return 0;

    if (strcmp(key, "Vars") == 0) {
        Driver* pDriver = static_cast<Driver*>(this);
        MapStringToVariant fields;
        int tableSize = luaL_getn(L, -1);
        for (int i = 1; i <= tableSize; i++) {
            /// Имя переменной в ВМ
            const char* readVarName = NULL;
            const char* writeVarName = NULL;
            // идентификатор переменной в ВМ
            int readVarID = 0;
            int writeVarID = 0;

            lua_rawgeti(L, -1, i);
            lua_pushnil(L);
            // stack now contains: -1 => nil; -2 => table
            while (lua_next(L, -2)) {
                const char* keyField = lua_tostring(L, -2);
                if (strcmp(keyField, "ReadPath") == 0)
                    readVarName = lua_tostring(L, -1);
                else if (strcmp(keyField, "ReadId") == 0)
                    readVarID = lua_tointeger(L, -1);
                else if (strcmp(keyField, "WritePath") == 0)
                    writeVarName = lua_tostring(L, -1);
                else if (strcmp(keyField, "WriteId") == 0)
                    writeVarID = lua_tointeger(L, -1);
                else {
                    OpcUa_VariantHlp& value = fields[keyField];
                    OpcUa_BuiltInTypeHlp typeHlp;
                    ReadLuaValue(value, L, -1, typeHlp);
                }
                lua_pop(L, 1);
            }

            lua_pop(L, 1);
            pDriver->AddChannel(readVarID, readVarName, writeVarID, writeVarName, fields);
            fields.clear();
        }
        return 0;
    }

    return BaseFB<Driver>::setField(L);
}

template<class Driver>
int DriverFB<Driver>::call(lua_State*) {
    Driver* pFB = static_cast<Driver*>(this);

    pFB->Execute();
    return 1;
}

template<class Driver>
inline void DriverFB<Driver>::CalcRecconectInterval(void) {
    unsigned int tmpReconnectInterval;
    if (_ReconnectCount != 0)
        tmpReconnectInterval = _ReconnectInterval * 2;
    else
        tmpReconnectInterval = _BeginReconnectInterval;

    if (tmpReconnectInterval > _MaxReconnectInterval)
        _ReconnectInterval = _MaxReconnectInterval;
    else
        _ReconnectInterval = tmpReconnectInterval;
    _ReconnectCount++;
}

template<class Driver>
void DriverFB<Driver>::SetErrorText( const std::string& errorText) {
    if (_ErrorTextItemId != 0) {
        OpcUa_VariantHlp value;
        value.SetString(errorText);
        _DataProvider->WriteSingleValue(_ErrorTextItemId, value);
    }
}

template<class Driver>
void DriverFB<Driver>::UpdateFaultState(bool fault, const std::string& errorText) {
    if (_DataProvider == NULL)
        return;
    _DataProvider->SetProtocolFaultStatus(fault, fault);
    if (fault == _lastFault)
        return;

    _lastFault = fault;
    if (_FaultItemId != 0) {
        OpcUa_VariantHlp value;
        value.SetBool(fault);
        _DataProvider->WriteSingleValue(_FaultItemId, value);
    }
    SetErrorText(errorText);
    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, 
                "Task %d Fault changed to %d: %s",
                _DataProvider->GetSerialTaskIndex(),
                fault ? 1 : 0,
                errorText.c_str());
}

/// В зависимости от настроек режима способа записи переменных драйвера, возвращает признак необходимости записи
/// переменной Режимы способа записи см. ProtocolWriteCondition
template<class Driver>
bool DriverFB<Driver>::IsNeedWriteValue(const OpcUa_VariantHlp& value,
                                        DriverVarDsc& dsc,
                                        bool allowWriteBadValues,
                                        bool writeIfNotChanged) {
    if (dsc.WriteType.Type == OpcUaType_DataValue &&
        (value.GetType() != OpcUaType_DataValue || 
            value.Value.DataValue->StatusCode == OpcUa_BadWaitingForInitialData ||
         !allowWriteBadValues && OpcUa_IsNotGood(value.Value.DataValue->StatusCode) ||
         value.Value.DataValue->Value.Datatype == OpcUaType_Null))
        return false;

    bool needToWrite = dsc.NeedToWrite || _writeCondition == pwcByCondition || _writeCondition == pwcByTime || writeIfNotChanged;

    if (_writeCondition == pwcByChange || _writeCondition == pwcByChangeTime ||
        _writeCondition == pwcByChangeStatusCode || _writeCondition == pwcByChangeStatusCodeTime) {
        if (dsc.WriteType.Type == OpcUaType_DataValue) {
            int64_t time = OpcUa_DateTime_ToInt64(value.Value.DataValue->SourceTimestamp);
            if ((_writeCondition == pwcByChangeTime || _writeCondition == pwcByChangeStatusCodeTime) &&
                    time != dsc.ValueToWriteTime ||
                dsc.ValueToWrite.GetType() == OpcUaType_DataValue &&
                    (OpcUa_VariantHlp::CompareVariants(value.Value.DataValue->Value, dsc.ValueToWrite.GetRawValue().DataValue->Value) != 0 ||
                     (_writeCondition == pwcByChangeStatusCode || _writeCondition == pwcByChangeStatusCodeTime) &&
                         OpcUa_IsGood(value.Value.DataValue->StatusCode) &&
                         OpcUa_IsNotGood(dsc.ValueToWrite.GetRawValue().DataValue->StatusCode)) ||
                dsc.ValueToWrite.GetType() != OpcUaType_DataValue &&
                    OpcUa_VariantHlp::CompareVariants(value.Value.DataValue->Value, dsc.ValueToWrite) != 0)
                needToWrite = true;
        } else
            needToWrite |= OpcUa_VariantHlp::CompareVariants(value, dsc.ValueToWrite) != 0;
    }

    if (needToWrite) {
        if (dsc.WriteType.Type == OpcUaType_DataValue) {
            dsc.ValueToWrite.CopyFrom(value.Value.DataValue->Value);
            dsc.ValueToWriteTime = OpcUa_DateTime_ToInt64(value.Value.DataValue->SourceTimestamp);
        } else {
            dsc.ValueToWrite.CopyFrom(value);
            dsc.ValueToWriteTime = getInt64FileTime();
        }
        dsc.NeedToWrite = false;
    }
    return needToWrite;
}


template<class Driver>
void DriverFB<Driver>::InitVar(DriverVarDsc& dsc) {
    if (dsc.WriteId != 0)  // Запись переменной
    {
        _DataProvider->GetItemTypeHlp(dsc.WriteId, dsc.WriteType);
        _DataProvider->ReadValue(dsc.WriteId, 0, dsc.WritePath, dsc.WriteType, &dsc.ValueToWrite);
    }
    if (dsc.Id) {
        _DataProvider->GetItemTypeHlp(dsc.Id, dsc.ReadType);
    }
}
