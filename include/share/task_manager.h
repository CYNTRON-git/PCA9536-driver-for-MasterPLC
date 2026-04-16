#pragma once

#include <lua.hpp>
//#include <share/opcua_json.h>

class LuaType;
class LuaInfo;
struct DriverVarDsc;
class IDataSource;

#ifndef luaL_getn
#    define luaL_getn(L, i) ((int)lua_rawlen(L, i))  // For lua 5.3
#endif

static const char* SUBSCRIPTION = "SUBSCRIPTION";

namespace mplc {
    struct JsonWrapper;
    struct lua_proxy_t {
        virtual ~lua_proxy_t() {}
        virtual void lua_set(lua_State* L) const = 0;
        virtual void lua_get(lua_State* L) = 0;
        virtual void to_json(mplc::JsonWrapper& json) const = 0;
    };
    namespace lua {
        class lua_function;
    }
}  // namespace mplc

namespace WriteDataOperation {
    enum WriteDataOperationEnum { Impulse = 6 };
}

struct WriteDataRec final {
    // typedef int self_get_set;
    int64_t _itemId;
    std::string _path;
    int _operation;
    int _typeHash;
    int _parameter;  // Duration for impulse
    int64_t _time_to_wait;
    OpcUa_VariantHlp _value;
    OpcUa_BuiltInTypeHlp _valueType;

    static WriteDataRec makeRecord(const DriverVarDsc& dsc);
    WriteDataRec(): _itemId(0), _parameter(0), _time_to_wait(0) {
        _operation = 0;
        _typeHash = 0;
    }

    WriteDataRec(int64_t itemId, const std::string& path, int operation, int typeHash): _time_to_wait(0) {
        SetDsc(itemId, path, operation, typeHash);
    }

    void SetDsc(int64_t itemId, const std::string& path, int operation, int typeHash) {
        _itemId = itemId;
        _path = path;
        _operation = operation;
        _typeHash = typeHash;
    }
};

class LuaDataProvider;
struct CallPOURec;
class LuaTask /*: mplc::vm::Task*/ {
public:
    typedef std::list<mplc::lib::shared_ptr<CallPOURec>> CallPOURecList;
    MPLCSHARE_API LuaTask(int taskId);
    MPLCSHARE_API virtual ~LuaTask();
    MPLCSHARE_API OpcUa_StatusCode CallPOUs(CallPOURecList& recs);
    MPLCSHARE_API LuaDataProvider* GetDataProvider(void) {
        return _DataProvider;
    }

protected:
    MPLCSHARE_API OpcUa_StatusCode AttachSTProcessor(const char* name, uint32_t callback_mode = 0xffff);
    virtual OpcUa_StatusCode OnSTEvent(STProcessorMode mode) = 0;
    MPLCSHARE_API OpcUa_StatusCode CallReadVarSimpleValue(const int64_t id,
                                                          const int typeHash,
                                                          const char* path,
                                                          const size_t pathLen) const;

    // Стек lua
    // TODO не очень хорошо, lua_State *L повторяется в  _DataProvider. надо убрать отсюда
    // lua_State* L;
    LuaDataProvider* _DataProvider;
    int _taskId;
    MPLCSHARE_API static int STProcessor(void* data, STProcessorMode mode, int taskId);

public:
    /*mplc::vm::TaskItemMonitor& monitor() override {
        return item_monitor;
    }*/

protected:
    // mplc::vm::LuaItemMonitor item_monitor;
    int _stProcessorIndex;
};
