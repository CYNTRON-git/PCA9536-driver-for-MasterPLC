#pragma once
#include "scada_object.h"
#include "scada_module.h"

namespace mplc { namespace api {

    struct ScadaProtocol : ScadaObject {
    public:
        ScadaProtocol(const ScadaProtocol&) = delete;
        ScadaProtocol& operator=(const ScadaProtocol&) = delete;
        enum DriverStates { NotConnected = 0, Connected = 1 };
        MPLC_OBJECT(ScadaProtocol);
        REF_TO ItemId;
        ProtocolWriteCondition WriteCondition{pwcByChange};
        BOOL ExecuteOnSlave{};
        BOOL WriteOnSlave{};
        BOOL TaskFaultOnAllModulesFault{};
        BOOL FaultOnTaskFault{};
        LINT ResetFaultDelayInSlave{};
        ScadaProtocol() = default;
        virtual ~ScadaProtocol() = default;
        virtual void Execute() = 0;

        virtual ScadaModule* Create(const vm::IOModule* module) {
            return nullptr;
        }
        virtual ScadaChannel* Create(const vm::Channel* channel) {
            return nullptr;
        }
        virtual ScadaParameter* Create(const vm::Variable* variable) {
            return nullptr;
        }
        MPLCSHARE_API virtual void InitChilds(const vm::PtNode* folder);
        MPLCSHARE_API virtual void InitChannelGroup(const vm::ChannelGroup* group);
        MPLCSHARE_API virtual void Init() {}
        MPLCSHARE_API bool isExecute();
        MPLCSHARE_API bool isConnect();
        MPLCSHARE_API bool isWrite();
        MPLCSHARE_API void InitInternal(LuaDataProvider* provider);
        MPLCSHARE_API void SetFaultState(bool fault, const std::string& errorText = "");
        /*
         * \brief  Обновляет состояние протокола. В отличие от SetFaultState errorText устанавливается
         * независимо от последнего состояния протокола
         * \param fault Содержит текущее состояние ошибки, true если произошёл сбой
         * \param errorText Текст ошибки
         */
        MPLCSHARE_API void UpdateFaultState(bool fault, const std::string& errorText = "");

        LuaDataProvider* LuaProvider() const {
            return _lua_provider;
        }
        bool isFailed() const {
            return _last_fault;
        }
        bool IsNeedWrite(ScadaChannel& ch, const OpcUa_VariantHlp& value) {
            return ch.IsNeedWrite(value, WriteCondition);
        }
        void SetEnO(bool v) {
            __EnO = v;
        }
        bool GetEnO() const {
            return __EnO;
        }
        int TaskIdx() const {
            return __TaskIdx;
        }
        // Return count of milliseconds
        uint32_t TaskPeriod() const {
            return __TaskPeriod;
        }

        virtual void lock() {}
        virtual void unlock() {}

    private:
        friend struct ApiStorage;
        BOOL __Internal{};
        BOOL __EnO{};
        LuaDataProvider* _lua_provider{};
        bool _last_fault{};
        bool _last_execute{};
        bool _last_write{};
        int __TaskIdx{-1};
        uint32_t __TaskPeriod{100};
        lib::optional<ScadaParameter> ExecuteItem, ConnectItem, WriteItem, FaultItem, ErrorTextItem;
    };
}}  // namespace mplc::api

MPLC_EXPORT_PUBLIC_API(MPLCSHARE_API, mplc::api::ScadaProtocol);
