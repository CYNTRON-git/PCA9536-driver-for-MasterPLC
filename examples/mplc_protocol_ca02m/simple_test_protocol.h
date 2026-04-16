#pragma once
#include <mplc/api.h>

#include "emulator.h"

class SimpleTestProtocol final : public mplc::api::ScadaProtocol {
private:
    std::vector<mplc::api::ScadaChannel*> channels;
    Emulator *emulator;
    uint8_t mask;
    BOOL USB_MOD_CONNED = FALSE;

public:
    MPLC_OBJECT(SimpleTestProtocol);
    SimpleTestProtocol();
    ~SimpleTestProtocol();
    void Execute() override;
    mplc::api::ScadaChannel* Create(const mplc::vm::Channel* channel) override;
    void Init() override;
};
