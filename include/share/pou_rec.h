#pragma once
#include <string>
#include <opcua.h>
#include <mplc/libs/smart_ptr.hpp>
#include "opcua_variant_hlp.h"

class IDataSource;
class CSemaphore;

namespace mplc { namespace lua {
    class lua_function;
}}  // namespace mplc::lua
struct CallPOURecParam {
    /* CallPOURecParam& operator=(const CallPOURecParam& source) {
        Name = source.Name;
        TypeHash=source.TypeHash;
        ValueType=source.ValueType;
        Value = source.Value;
        direction=source.direction;
        return *this;
    }*/
    OpcUa_VariantHlp Value;
    std::string Name;
    const mplc::vm::VmType* Type{};
    // TODO! заменить на использование Type
    // OpcUa_BuiltInType ValueType;
    // TODO! заменить на использование Type
    int TypeHash;
    enum { In, Out } direction;
};

struct CallPOURec {
    typedef mplc::lib::shared_ptr<CallPOURec> ptr;
    typedef std::vector<CallPOURecParam> CallPOURecParamArr;
    CallPOURecParamArr Params;
    CallPOURecParamArr OutParams;
    std::string SourceInfo;  // user@ip_addr
    std::string Path;
    int64_t ItemId;
    OpcUa_StatusCode StatusCode;
    int TypeHash;
    int CallType;
    CSemaphore* SemCompleted{};
    IDataSource* DataSource{};
    virtual ~CallPOURec() {
        // SemCompleted = NULL;
        // DataSource = NULL;
    }

    CallPOURec(): ItemId(0), StatusCode(0), TypeHash(0), CallType(0) /*, SemCompleted(NULL), DataSource(NULL)*/ {}
    OpcUa_StatusCode call(mplc::lua::lua_function& func);
};
