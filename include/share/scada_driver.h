//#pragma once
//#include <share/scada_types.h>
//#include "drivers/drv_user.h"
//
//struct DriverVarDsc {
//    enum { Input = 0, Output = 1, InOut = 2 };
//    BYTE Direction;  // Направление передачи. 0 - входная переменная (читаем из драйвера), 1 -
//                     // выходная переменная (пишем в драйвер), 2 - вход/выход
//    OpcUa_VariantHlp Value;
//    int64_t Id;
//    std::string Path;
//    int64_t WriteId;
//    std::string WritePath;
//    OpcUa_BuiltInTypeHlp ReadType;
//    OpcUa_BuiltInTypeHlp WriteType;
//
//    bool Changed;  // Признак того что переменная изменена по подписке
//    bool NeedToWrite;  // Признак того что переменная должна быть отправлена независимо от того
//                       // изменилось или нет значение
//    OpcUa_VariantHlp ValueToWrite;  //Значение для записи
//    int64_t ValueToWriteTime;       //Значение для записи
//    DriverVarDsc() {
//        Direction = 0;
//        Id = 0;
//        WriteId = 0;
//        Value.ArrayType = 0;
//        Value.Datatype = OpcUaType_Null;
//        ValueToWrite.ArrayType = 0;
//        ValueToWrite.Datatype = OpcUaType_Null;
//        NeedToWrite = false;
//        Changed = false;
//        ValueToWriteTime = 0;
//    }
//    DriverVarDsc(int64_t readVarID,
//                 const char* readVarPath,
//                 int64_t writeVarID,
//                 const char* writeVarPath) {
//        ValueToWriteTime = 0;
//        if(readVarID != 0 && writeVarID != 0)
//            Direction = 2;
//        else
//            Direction = writeVarID != 0 ? 1 : 0;
//
//        Id = readVarID;
//        if(readVarPath != NULL) Path = readVarPath;
//        WriteId = writeVarID;
//        if(writeVarPath != NULL) WritePath = writeVarPath;
//
//        Value.ArrayType = 0;
//        Value.Datatype = OpcUaType_Null;
//        ValueToWrite.ArrayType = 0;
//        ValueToWrite.Datatype = OpcUaType_Null;
//        NeedToWrite = false;
//        Changed = false;
//    }
//};
//
//typedef std::map<std::string, OpcUa_VariantHlp> MapStringToVariant;
//
//inline int GetIntFieldFromMap(const MapStringToVariant& m,
//                              const std::string& key,
//                              int defaultValue = 0) {
//    MapStringToVariant::const_iterator it = m.find(key);
//    if(it == m.end()) return defaultValue;
//
//    int i;
//    it->second.GetInt(i);
//    return i;
//}
//
//inline std::string GetStringFieldFromMap(const MapStringToVariant& m,
//                                         const std::string& key,
//                                         const std::string defaultValue = std::string()) {
//    MapStringToVariant::const_iterator it = m.find(key);
//    if(it == m.end()) return defaultValue;
//
//    std::string res;
//    it->second.GetString(res);
//    return res;
//}
//class DriverFB : public BaseLuaObj {
//    MPLC_FB_PARAMS(
//       F(FaultItemId, LINT),
//       F(ErrorTextItemId, LINT),
//       F(ExecuteItemId, LINT),
//       F(ConnectItemId, LINT),
//       F(WriteItemId, LINT),
//       F(WriteCondition, INT),
//       F(ExecuteOnSlave, BOOL))
//protected:
//    // Начальный интервал времени переподключения (с)
//    static const unsigned int _BeginReconnectInterval = 5;
//    // Максимальное интервал времени переподключения (с)
//    static const unsigned int _MaxReconnectInterval = 60;
//
//    enum DriverStates { NotConnected = 0, Connected = 1 };
//
//    LuaDataProvider* _DataProvider;
//    // Признак того что в LuaDataProvider можно загружать данные о ВМ
//    bool _DPInit;
//    int64_t _FaultItemId;
//    int64_t _ErrorTextItemId;
//    int64_t _ExecuteItemId;
//    int64_t _ConnectItemId;
//    int64_t _WriteItemId;
//    enum ProtocolWriteCondition _writeCondition;
//    bool _executeOnSlave;
//
//    bool _lastFault;
//    bool _lastExecute;
//    bool _lastWrite;
//    bool _wasConnectAttempt;  //Была хотя бы одна попытка подключения
//
//    time_t _LastTimeToConnect;
//    // Интервал в секундах для попыток переподключения если сесия была разорвана
//    unsigned int _ReconnectInterval;
//    // Кол-во попыток переподключения если сесия была разорвана
//    unsigned int _ReconnectNum;
//    // Счетчик попыток переподключения если сесия была разорвана
//    unsigned int _ReconnectCount;
//
//    virtual void AddChannel(int64_t /*readVarID*/,
//                            const char* /* readVarPath */,
//                            int64_t /* writeVarID */,
//                            const char* /* writeVarPath */,
//                            const MapStringToVariant& /* fields */) {}
//    virtual void CalcRecconectInterval() {
//        unsigned int tmpReconnectInterval;
//        if(_ReconnectCount != 0)
//            tmpReconnectInterval = _ReconnectInterval * 2;
//        else
//            tmpReconnectInterval = _BeginReconnectInterval;
//
//        if(tmpReconnectInterval > _MaxReconnectInterval)
//            _ReconnectInterval = _MaxReconnectInterval;
//        else
//            _ReconnectInterval = tmpReconnectInterval;
//        _ReconnectCount++;
//    }
//    void UpdateFaultState(bool fault, const std::string& errorText = "");
//    bool isExecute() {
//        if(_ExecuteItemId == 0) return false;
//        OpcUa_VariantHlp value;
//        _DataProvider->ReadValue(_ExecuteItemId, 0, std::string(), OpcUaType_Boolean, &value);
//        bool bValue = false;
//        value.GetBool(bValue);
//        bool res = bValue && !_lastExecute;
//        _lastExecute = bValue;
//        return res;
//    }
//    bool isConnect() const {
//        if(!_executeOnSlave && IsSystemInReserv()) return false;
//        if(IsDisableRemotePorts()) return false;
//        if(_ConnectItemId == 0) return true;
//        OpcUa_VariantHlp value;
//        _DataProvider->ReadValue(_ConnectItemId, 0, std::string(), OpcUaType_Boolean, &value);
//        bool res = false;
//        value.GetBool(res);
//        return res;
//    }
//    bool isWrite() {
//        if(!_executeOnSlave && IsSystemInReserv()) return false;
//        if(IsDisableRemotePorts()) return false;
//        if(_WriteItemId == 0) return true;
//        OpcUa_VariantHlp value;
//        _DataProvider->ReadValue(_WriteItemId, 0, std::string(), OpcUaType_Boolean, &value);
//        bool bValue = false;
//        value.GetBool(bValue);
//        bool res = bValue && !_lastWrite;
//        _lastWrite = bValue;
//        return res;
//    }
//    bool IsNeedWriteValue(const OpcUa_VariantHlp& value,
//                          DriverVarDsc& dsc,
//                          bool needWriteByCondition) {
//        if(dsc.WriteType.Type == OpcUaType_DataValue &&
//           (value.GetType() != OpcUaType_DataValue ||
//            OpcUa_IsNotGood(value.Value.DataValue->StatusCode) ||
//            value.Value.DataValue->Value.Datatype == OpcUaType_Null))
//            return false;
//
//        bool needToWrite = dsc.NeedToWrite || needWriteByCondition || _writeCondition == pwcByTime;
//
//        if(_writeCondition == pwcByChange || _writeCondition == pwcByChangeTime) {
//            if(dsc.WriteType.Type == OpcUaType_DataValue) {
//                int64_t time = OpcUa_DateTime_ToInt64(value.Value.DataValue->SourceTimestamp);
//                if(_writeCondition == pwcByChangeTime && time != dsc.ValueToWriteTime ||
//                   OpcUa_VariantHlp::CompareVariants(value.Value.DataValue->Value,
//                                                     dsc.ValueToWrite) != 0)
//                    needToWrite = true;
//            } else
//                needToWrite |= OpcUa_VariantHlp::CompareVariants(value, dsc.ValueToWrite) != 0;
//        }
//
//        if(needToWrite) {
//            if(dsc.WriteType.Type == OpcUaType_DataValue) {
//                dsc.ValueToWrite.CopyFrom(value.Value.DataValue->Value);
//                dsc.ValueToWriteTime = OpcUa_DateTime_ToInt64(
//                    value.Value.DataValue->SourceTimestamp);
//            } else {
//                dsc.ValueToWrite.CopyFrom(value);
//                dsc.ValueToWriteTime = getInt64FileTime();
//            }
//        }
//        return needToWrite;
//    }
//    void InitVar(DriverVarDsc& dsc) {
//        if(dsc.WriteId != 0)  // Запись переменной
//        {
//            _DataProvider->GetItemTypeHlp(dsc.WriteId, dsc.WriteType);
//            _DataProvider->ReadValue(dsc.WriteId,
//                                     0,
//                                     dsc.WritePath,
//                                     dsc.WriteType,
//                                     &dsc.ValueToWrite);
//        }
//        if(dsc.Id) { _DataProvider->GetItemTypeHlp(dsc.Id, dsc.ReadType); }
//    }
//
//    virtual void InitedInternal(lua_State*) {
//        //_DataProvider = LuaDataProvider::AddLuaDataProvider(L);
//    }
//
//public:
//    DriverFB(lua_State* L, const std::string& name) {
//        _DPInit = false;
//        _FaultItemId = 0;
//        _lastFault = false;
//        _wasConnectAttempt = false;
//        _LastTimeToConnect = 0;
//        _ReconnectInterval = 5;
//        _ReconnectNum = 5;
//        _ReconnectCount = 0;
//        _ErrorTextItemId = 0;
//        _ExecuteItemId = 0;
//        _lastExecute = false;
//        _lastWrite = false;
//        _ConnectItemId = 0;
//        _WriteItemId = 0;
//        _writeCondition = pwcByChange;
//        _executeOnSlave = false;
//        _DataProvider = LuaDataProvider::AddLuaDataProvider(L);
//    }
//    virtual int setField(lua_State* L) {
//        const char* key = lua_tostring(L, -2);
//        if(strcmp(key, "FaultItemId") == 0) {
//            _FaultItemId = lua_tointeger(L, -1);
//            return 0;
//        }
//        if(strcmp(key, "ErrorTextItemId") == 0) {
//            _ErrorTextItemId = lua_tointeger(L, -1);
//            return 0;
//        }
//        if(strcmp(key, "ExecuteItemId") == 0) {
//            _ExecuteItemId = lua_tointeger(L, -1);
//            return 0;
//        }
//        if(strcmp(key, "ConnectItemId") == 0) {
//            _ConnectItemId = lua_tointeger(L, -1);
//            return 0;
//        }
//        if(strcmp(key, "WriteItemId") == 0) {
//            _WriteItemId = lua_tointeger(L, -1);
//            return 0;
//        }
//        if(strcmp(key, "WriteCondition") == 0) {
//            _writeCondition = (ProtocolWriteCondition)lua_tointeger(L, -1);
//            return 0;
//        }
//        if(strcmp(key, "ExecuteOnSlave") == 0) {
//            _executeOnSlave = (bool)lua_toboolean(L, -1);
//            return 0;
//        }
//        if(strcmp(key, "Vars") == 0) {
//            Driver* pDriver = static_cast<Driver*>(this);
//            MapStringToVariant fields;
//            int tableSize = luaL_getn(L, -1);
//            for(int i = 1; i <= tableSize; i++) {
//                /// Имя переменной в ВМ
//                const char* readVarName = NULL;
//                const char* writeVarName = NULL;
//                // идентификатор переменной в ВМ
//                int readVarID = 0;
//                int writeVarID = 0;
//
//                lua_rawgeti(L, -1, i);
//                lua_pushnil(L);
//                // stack now contains: -1 => nil; -2 => table
//                while(lua_next(L, -2)) {
//                    const char* keyField = lua_tostring(L, -2);
//                    if(strcmp(keyField, "ReadPath") == 0)
//                        readVarName = lua_tostring(L, -1);
//                    else if(strcmp(keyField, "ReadId") == 0)
//                        readVarID = lua_tointeger(L, -1);
//                    else if(strcmp(keyField, "WritePath") == 0)
//                        writeVarName = lua_tostring(L, -1);
//                    else if(strcmp(keyField, "WriteId") == 0)
//                        writeVarID = lua_tointeger(L, -1);
//                    else {
//                        OpcUa_VariantHlp& value = fields[keyField];
//                        OpcUa_BuiltInTypeHlp typeHlp;
//                        ReadLuaValue(value, L, -1, typeHlp);
//                    }
//                    lua_pop(L, 1);
//                }
//
//                lua_pop(L, 1);
//                pDriver->AddChannel(readVarID, readVarName, writeVarID, writeVarName, fields);
//                fields.clear();
//            }
//            return 0;
//        }
//
//        return BaseFB<Driver>::setField(L);
//    }
//
//    virtual int call(lua_State* L) {
//        Driver* pFB = static_cast<Driver*>(this);
//
//        pFB->Execute();
//        return 1;
//    }
//};
