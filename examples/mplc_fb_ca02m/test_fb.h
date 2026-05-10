#pragma once
#include <mplc/api.h>
#include "../common/ca02m_hw.h"

class TestFB : public mplc::api::ScadaFB {
    BOOL Blue_Led;
    BOOL Red_Led;
    BOOL Buzzer;
    BOOL USB_Power;
    BOOL Dout;

    BOOL was_USB_Power;
    BOOL USB_Reset;
    INT  USB_Counter;

    uint8_t last_mask = cyntron_ca02m::kAllOffMask;
    bool outputs_initialized = false;
    cyntron_ca02m::Pca9536Controller pca9536_;
    cyntron_ca02m::UsbPowerController usb_power_;
public:
    MPLC_OBJECT(TestFB);
    void Execute() override;
};

