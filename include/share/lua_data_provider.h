#pragma once
#include <share/opcua_json.h>
#include "opcua_variant_hlp.h"
#include "mplc/lua/lua_function.h"
#include "mplc/vm/vminfo.h"
#include <mplc/lua_to_msgpack.h>
#include <msgpack/object.hpp>

class MPLCSHARE_API LuaDataProvider {
    struct impl;
    impl* pimpl;

protected:
    void Init(lua_State* pL);
    // Читаем переменную из Lua
    OpcUa_StatusCode ReadJSONRaw(OpcUa_Variant* pValue) const;
    int _taskId;
    int _serialTaskIndex;

public:
    lua_State* L;
    //    LuaInfo LInfo;

    LuaDataProvider();

    ~LuaDataProvider();
    // [[deprecated("Use VMInfo::GetType")]]
    static OpcUa_BuiltInType GetItemType(int64_t id) {
        OpcUa_BuiltInTypeHlp type;
        OpcUa_CheckError(mplc::vm::GetItemType(type, id));
        return type.Type;
    }
    static OpcUa_StatusCode GetItemTypeHlp(int64_t id, const std::string& path, OpcUa_BuiltInTypeHlp& ret) {
        return mplc::vm::GetItemType(ret, id, path);
    }
    static OpcUa_StatusCode GetItemTypeHlp(int64_t id, OpcUa_BuiltInTypeHlp& ret) {
        return mplc::vm::GetItemType(ret, id);
    }

    // Создает, инициализирует провайдер данных, если провайдер рранее не был создан
    static LuaDataProvider* AddLuaDataProvider(lua_State* L);
    static LuaDataProvider* AddLuaDriverProvider(lua_State* L, void* protocol_data, mplc::lib::string_view name);
    static void DeleteLuaDataProvider(lua_State* L);
    mplc::lua::lua_function LuaFunction(const std::string& funName) const;
    OpcUa_StatusCode WriteValues(const std::vector<WriteDataRec>& recsToWrite) const;
    OpcUa_StatusCode WriteValues(const std::vector<WriteDataRec>& recsToWrite,
                                 std::vector<WriteDataRec>* recsToWriteNext) const;
    OpcUa_StatusCode WriteSingleValue(int64_t itemId, const OpcUa_VariantHlp& value) const;
    OpcUa_StatusCode WriteSingleValue(const mplc::vm::ItemID& itemId, const OpcUa_VariantHlp& value) const;
    //	MPLCSHARE_API static OpcUa_StatusCode WriteValues(lua_State *L, const std::vector<WriteDataRec>& recsToWrite);
    //	MPLCSHARE_API static OpcUa_StatusCode ReadValue(lua_State *L, const int64_t id, const int typeHash, const
    // std::string &path, const OpcUa_BuiltInTypeHlp& typeHlp, OpcUa_VariantHlp* pValue);
    OpcUa_StatusCode ReadValue(const mplc::vm::ItemID& item_id,
                               const int typeHash,
                               const OpcUa_BuiltInTypeHlp& typeHlp,
                               OpcUa_VariantHlp* pValue,
                               GetValueFlags::GetValueFlagsEnum flags = GetValueFlags::ReadTimeAsLinuxTime) const;

    mplc::lua::ChangeType UpdateValue(const mplc::vm::ItemID& variable, msgpack::object_handle& oh) const;
    OpcUa_StatusCode WriteValue(const mplc::vm::ItemID& item_id, const msgpack::object& val) const;

    // WARN: DEPRICATED use overloaded version with mplc::vm::ItemID
    OpcUa_StatusCode ReadValue(int64_t item_id,
                               const int typeHash,
                               const std::string& path,
                               const OpcUa_BuiltInTypeHlp& typeHlp,
                               OpcUa_VariantHlp* pValue,
                               GetValueFlags::GetValueFlagsEnum flags = GetValueFlags::ReadTimeAsLinuxTime) const {
        return ReadValue(mplc::vm::ItemID(item_id, path), typeHash, typeHlp, pValue, flags);
    }

    OpcUa_StatusCode ReadTableOPCVariantValueRaw(const mplc::vm::ItemID& item_id,
                                                 const int typeHash,
                                                 const OpcUa_BuiltInTypeHlp& typeHlp,
                                                 OpcUa_Variant* pValue) const;
    // WARN: DEPRICATED use overloaded version with mplc::vm::ItemID
    OpcUa_StatusCode ReadTableOPCVariantValueRaw(const int64_t id,
                                                 const int typeHash,
                                                 const std::string& path,
                                                 const OpcUa_BuiltInTypeHlp& typeHlp,
                                                 OpcUa_Variant* pValue) const;

    OpcUa_StatusCode ReadValueFromLua(const mplc::vm::ItemID& item_id, const int typeHash) const;
    // WARN: DEPRICATED use overloaded version with mplc::vm::ItemID
    OpcUa_StatusCode ReadValueFromLua(int64_t id, int typeHash, mplc::lib::string_view path) const;
    OpcUa_StatusCode SetNeedAutolock(bool autoLock);
    OpcUa_StatusCode SetLock(bool lock);
    void SetProtocolFaultStatus(bool readFault, bool writeFault);
    int FindTaskParamIndex(int64_t itemId);
    OpcUa_StatusCode GetExternalParamValue(int64_t itemId, OpcUa_VariantHlp& v, const OpcUa_BuiltInTypeHlp& typeHlp);
    int GetSerialTaskIndex() {
        return _serialTaskIndex;
    }
    void* TaskData() const;
    std::string TaskName() const;

protected:
    // void MakeJSONArrayString(JsonWriter<JsonTextWriter<rapidjson::StringBuffer> >& writer, const int arrayLen, const
    // int id) const; void MakeJSONStructString(JsonWriter<JsonTextWriter<rapidjson::StringBuffer> >& writer,
    //                          const int id,
    //                          const LuaType* varType) const;
};

class DataProviderLock {
    LuaDataProvider* _p;

public:
    DataProviderLock(LuaDataProvider* p) {
        _p = p;
        if (p != nullptr)
            p->SetLock(true);
    }
    ~DataProviderLock() {
        if (_p != nullptr)
            _p->SetLock(false);
    }
};
