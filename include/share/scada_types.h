#pragma once
#include <mplc/libs/threads.hpp>
#include <mplc/libs/bind.hpp>
#include <share/mplcshare.h>
#include "lua/lua_addins.h"
#include "scada_type_traits.hpp"
#include <core/drivers/drv_user.h>

struct DriverVarDsc;

class MPLCSHARE_API BaseLuaObj {
    template<class T>
    friend class SCADA_API::ScadaObj;

protected:
    struct FBData data {};

    template<class T>
    struct DynamicFields : ScadaFields::IDynamicFields {
        int (T::*SetField)(const std::string&, lua_State*);
        int (T::*GetField)(const std::string&, lua_State*) const;

        DynamicFields(int (T::*setter)(const std::string&, lua_State*),
                      int (T::*getter)(const std::string&, lua_State*) const)
            : SetField(setter), GetField(getter) {}

        int set(const void* ptr, const boost::string_view& key, lua_State* L) override {
            const T* obj = static_cast<const T*>(ptr);
            return (obj->*GetField)(key.to_string(), L);
        }

        int get(void* ptr, const boost::string_view& key, lua_State* L) override {
            T* obj = static_cast<T*>(ptr);
            return (obj->*SetField)(key.to_string(), L);
        }
    };

public:
    virtual ~BaseLuaObj() = default;

    static const char* GetMetaParams() {
        return "";
    }

    BaseLuaObj();
    virtual bool check(lua_State* L, int key_pos);
    void SetEnO(bool v);
    void Init(lua_State* L);
    virtual void Inited();
    virtual void InitedInternal(lua_State*);
};

class MPLCSHARE_API ScadaStruct : public BaseLuaObj {};

class MPLCSHARE_API ScadaFB : public BaseLuaObj {
public:
    ScadaFB() = default;
    int call(lua_State*);
    virtual void Execute() = 0;

    virtual void PostExecute(lua_State* L) {}
};

class ScadaFBThreadSafe : public ScadaFB {
    MPLC_LOCKABLE_TYPE();

public:
    struct lock_guard {
        lock_guard(ScadaFBThreadSafe* fb): lock(fb->GetFieldsMutex()) {}

        mplc::lib::lock_guard<mplc::lib::mutex> lock;
    };
};

class MPLCSHARE_API ScadaProtocol : public BaseLuaObj {
public:
    struct MPLCSHARE_API Channels {
        using is_disable_json = int;
        ScadaProtocol& drv;

        Channels(ScadaProtocol& drv): drv(drv) {}
    };

    friend MPLCSHARE_API void get_lua_value(Channels& ch, lua_State* L);
    int call(lua_State*);

    MPLC_INIT_TYPELIST(ScadaProtocol)

    // MPLC_INHERIT_THIS_FIELDS(ScadaProtocol)
    LuaDataProvider* LuaProvider() const {
        return _DataProvider;
    }

protected:
    enum DriverStates { NotConnected = 0, Connected = 1 };

    MPLC_LUA_BINDINGS(IN(FaultItemId, LINT),
                      IN(ErrorTextItemId, LINT),
                      IN(ExecuteItemId, LINT),
                      IN(ConnectItemId, LINT),
                      IN(WriteItemId, LINT),
                      IN(WriteCondition, ProtocolWriteCondition),
                      IN(ExecuteOnSlave, BOOL),
                      IN(WriteOnSlave, BOOL),
                      IN(TaskFaultOnAllModulesFault, BOOL),
                      IN(FaultOnTaskFault, BOOL),
                      IN(ResetFaultDelayInSlave, LINT),
                      IN(Vars, Channels))

    struct ReadChannel {
        int64_t id;
        std::string path;
        OpcUa_BuiltInTypeHlp type;

        ReadChannel(): id(0) {}
    };

    struct WriteChannel {
        int64_t id;
        std::string path;
        OpcUa_BuiltInTypeHlp type;
        OpcUa_VariantHlp val;

        WriteChannel(): id(0) {}
    };

protected:
    bool _lastFault;
    bool _lastExecute;
    bool _lastWrite;
    // bool _wasConnectAttempt;  //Была хотя бы одна попытка подключения

    virtual void AddChannel(const ReadChannel* /* readCh */,
                            const WriteChannel* /*writeCh*/,
                            const std::map<std::string, OpcUa_VariantHlp>& /* fields */) {}

    virtual void BaseInitChannel(const ReadChannel* read_chan, const WriteChannel* write_chan, DriverVarDsc& curVar);
    void UpdateFaultState(bool fault, const std::string& errorText = "");
    bool isExecute();
    bool isConnect() const;
    bool isWrite();
    bool IsNeedWriteValue(const OpcUa_VariantHlp& value,
                          DriverVarDsc& dsc,
                          bool allowWriteBadValues = false,
                          bool writeIfNotChanged = false);
    ScadaProtocol();
    void InitedInternal(lua_State*) override;
    OpcUa_StatusCode WriteValues(const std::vector<WriteDataRec>& recsToWrite);
    OpcUa_StatusCode ReadValue(const mplc::vm::ItemID& item_id,
                               const OpcUa_BuiltInTypeHlp& typeHlp,
                               OpcUa_VariantHlp* pValue,
                               GetValueFlags::GetValueFlagsEnum flags = GetValueFlags::None) const;
    // WARN: DEPRECATED
    OpcUa_StatusCode ReadValue(int64_t id,
                               const std::string& path,
                               const OpcUa_BuiltInTypeHlp& typeHlp,
                               OpcUa_VariantHlp* pValue,
                               GetValueFlags::GetValueFlagsEnum flags = GetValueFlags::None) const {
        return ReadValue(mplc::vm::ItemID(id, path), typeHlp, pValue, flags);
    }

    virtual void Execute() = 0;

private:
    // Признак того что в LuaDataProvider можно загружать данные о ВМ
    // bool _DPInit;

protected:
    // MPLC_DEPRECATED("Use LuaProvider()")
    LuaDataProvider* _DataProvider;
};

class ScadaProtocol2 : public ScadaProtocol {
public:
    virtual void AddModule(const mplc::vm::IOModule* module) {}
    virtual void AddVariable(const mplc::vm::Variable* var) {}
    MPLC_INHERIT_THIS_FIELDS(ScadaProtocol2)
    MPLC_LUA_BINDINGS(IN(ItemId, REF_TO))
    MPLCSHARE_API void InitedInternal(lua_State*) override;
};

inline void set_lua_value(const ScadaProtocol::Channels& ch, lua_State* L) {}
inline const char* get_lua_type(const ScadaProtocol::Channels*) {
    return nullptr;
}
