#include "CA_02m.h"
#include <ctime>
#include <fstream>

namespace mplc::cyntron {
namespace {
uint8_t effectiveMask(uint8_t mask) {
    std::ifstream input("/run/sa02m-hw-override/beeper.env");
    char buffer[128];
    input.read(buffer, sizeof(buffer));
    if (!input.eof() || input.bad()) return mask;
    return applyOverride(mask, {buffer, static_cast<size_t>(input.gcount())}, std::time(nullptr));
}
}

void CA_02m::Execute() {
    if (!isConnect() || !isExecute()) return;
    auto enabled = [this](const char* name) {
        auto it = chan.find(name);
        return it != chan.end() && it->second->OutVar.Get<bool>();
    };
    const auto mask = cyntron_ca02m::build_output_mask(
        enabled("BLUE_LED"), enabled("RED_LED"), enabled("BUZZER"), enabled("DOUT"));
    const auto now = OutputState::Clock::now();
    bool pca_ok = false;
    std::string error;
    try {
        pca_ok = outputs.update(effectiveMask(mask), now,
            [this](uint8_t value) { pca.writeOutputs(value); });
    } catch (const std::exception& ex) {
        pca_error = ex.what();
    }
    if (!pca_ok) error = pca_error;
    auto usb = chan.find("USB_POWER");
    if (usb != chan.end()) {
        try {
            usb_helper.set(usb->second->OutVar.Get<bool>());
        } catch (const std::exception& ex) {
            if (!error.empty()) error += "; ";
            error += ex.what();
        }
    }
    SetFaultState(!error.empty(), error);
    if (error.empty()) AcknowledgeAllChanges();
}

api::ScadaChannel* CA_02m::Create(const vm::Channel* channel) {
    auto* ch = new api::ScadaChannel();
    ch->BaseInit(channel);
    Register(ch);
    chan[std::string(channel->getTranslitedName())] = ch;
    return ch;
}
}
MPLC_PROTOCOL_TYPE(CA_02m, mplc::cyntron::CA_02m);
