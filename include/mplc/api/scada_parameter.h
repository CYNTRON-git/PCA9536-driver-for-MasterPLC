#pragma once
#include <string>
#include <opcua.h>
#include <share/config.h>
#include <share/opcua_variant_hlp.h>
#include <mplc/vm/item_id.h>

//#include "mplc/vm/vminfo.h"

namespace mplc { namespace vm {
    struct Variable;
}}  // namespace mplc::vm

class LuaDataProvider;

namespace mplc { namespace api {
    struct ScadaParameter {
        const vm::Variable* m_var{};
        //  std::vector<lib::string_view> m_path;
        //  OpcUa_BuiltInTypeHlp Type;
        OpcUa_VariantHlp Value;
        MPLCSHARE_API OpcUa_StatusCode Write(LuaDataProvider* provider) const;
        MPLCSHARE_API OpcUa_StatusCode Write(LuaDataProvider* provider, const OpcUa_VariantHlp& val) const;

        MPLCSHARE_API OpcUa_StatusCode Read(LuaDataProvider* provider);

        MPLCSHARE_API OpcUa_StatusCode Read(LuaDataProvider* provider, OpcUa_VariantHlp& val);

        MPLCSHARE_API const OpcUa_BuiltInTypeHlp Type() const;
        MPLCSHARE_API const vm::VmType* vmType() const;
        MPLCSHARE_API bool IsSystemParam() const;
        template<class T>
        OpcUa_StatusCode Set(const T& val) {
            return Value.Set(val);
        }
        template<class T>
        T Get(const T& def_val = T{}) {
            return Value.Get<T>(def_val);
        }

        void Set(const OpcUa_VariantHlp& new_val) {
            Value = new_val;
        }
        const OpcUa_VariantHlp& Get() const {
            return Value;
        }
        ScadaParameter() {}
        ScadaParameter(int64_t id, std::string path = "");
        ScadaParameter(const vm::ItemID& item_id);
        ScadaParameter(const ScadaParameter& param): m_var(param.m_var), Value(param.Value) {}
        ScadaParameter& operator=(const ScadaParameter& param) {
            m_var = param.m_var;
            Value = param.Value;
            return *this;
        }
        ScadaParameter(ScadaParameter&& param) noexcept: m_var(std::move(param.m_var)) {
            Value.Swap(param.Value);
        }
        ScadaParameter& operator=(ScadaParameter&& param) noexcept {
            m_var = std::move(param.m_var);
            Value.Swap(param.Value);
            return *this;
        }
        MPLCSHARE_API void Init(const vm::Variable* var);
    };

}}  // namespace mplc::api
