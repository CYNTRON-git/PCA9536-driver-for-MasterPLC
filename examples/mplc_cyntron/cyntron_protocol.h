#pragma once
#include <mplc/api.h>
#include <share/scada_types.h>
#include "core/main.h"
namespace mplc { namespace cyntron {
    class CyntronProtocol final : public api::ScadaProtocol {
    public:
        MPLC_OBJECT(CyntronProtocol);
        CyntronProtocol(): cpu_load_state(InitGetCPULoad()) {}
        CyntronProtocol(const CyntronProtocol&) = delete;
        CyntronProtocol& operator=(const CyntronProtocol&) = delete;
        ~CyntronProtocol() override = default;
        
        void Init() override;
        void Execute() override;

        api::ScadaChannel* Create(const vm::Channel* channel) override;

    private:
        struct CpuLoadStateDeleter {
            void operator()(CpuLoadState* ptr) {
                DeleteCpuLoadState(ptr);
            }
        };
        std::unique_ptr<CpuLoadState, CpuLoadStateDeleter> cpu_load_state;
        std::unordered_map<std::string, api::ScadaChannel*> chan;
    };
}} // namespace mplc::cyntron

