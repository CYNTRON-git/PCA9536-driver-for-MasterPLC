#pragma once
#include <mplc/api.h>
#include <share/scada_types.h>
#include "gpio_helper.h"
#include "../common/ca02m_hw.h"
#include "output_state.h"
namespace mplc { namespace cyntron {    
    class CA_02m final : public api::ScadaProtocol {
    public:
       
        MPLC_OBJECT(CA_02m);
        
        CA_02m() = default;
        ~CA_02m() override = default;

        CA_02m(const CA_02m&) = delete;
        CA_02m& operator=(const CA_02m&) = delete;

        void Init() override {}
        void Execute() override;

        api::ScadaChannel* Create(const vm::Channel* channel) override;
    private:
        std::unordered_map<std::string, api::ScadaChannel*> chan;
        OutputState outputs;
        cyntron_ca02m::Pca9536Controller pca;
        std::string pca_error;
        USBResetHelper usb_helper;
    };
}} // namespace mplc::cyntron

