#pragma once
#include <mplc/api.h>
#include <share/scada_types.h>
#include "gpio_helper.h"
namespace mplc { namespace cyntron {    
    class CA_02m final : public api::ScadaProtocol {
    public:
       
        MPLC_OBJECT(CA_02m);
        
        CA_02m();
        ~CA_02m() override;

        CA_02m(const CA_02m&) = delete;
        CA_02m& operator=(const CA_02m&) = delete;

        void Init() override {}
        void Execute() override;

        api::ScadaChannel* Create(const vm::Channel* channel) override;
    private:
        std::unordered_map<std::string, api::ScadaChannel*> chan;
        int i2c_fd = -1;
        char last_mask;
        USBResetHelper usb_helper;
    };
}} // namespace mplc::cyntron

