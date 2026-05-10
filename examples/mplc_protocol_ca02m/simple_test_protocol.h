#pragma once
#include <mplc/api.h>

#include "../common/ca02m_hw.h"
#include "emulator.h"

class SimpleTestProtocol final : public mplc::api::ScadaProtocol {
private:
    std::vector<mplc::api::ScadaChannel*> channels;
    Emulator *emulator;
    uint8_t last_mask = cyntron_ca02m::kAllOffMask;
    BOOL USB_MOD_CONNED = FALSE;
    bool outputs_initialized = false;
    cyntron_ca02m::Pca9536Controller pca9536_;
    cyntron_ca02m::UsbPowerController usb_power_;

public:
    MPLC_OBJECT(SimpleTestProtocol);
    SimpleTestProtocol();
    ~SimpleTestProtocol();
    void Execute() override;
    mplc::api::ScadaChannel* Create(const mplc::vm::Channel* channel) override;
    void Init() override;
};
