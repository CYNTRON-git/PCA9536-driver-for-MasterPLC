#include "CA_02m.h"
#include "i2c_system.h"

#include <cstring>

namespace mplc { namespace cyntron {
    constexpr const char* FILENAME = "/dev/i2c-2";
    constexpr int DEVICE_ADDR = 0x41;
    constexpr char ALL_OFF_BYTE = 0x0f;
    constexpr char LOAD_CONTROL_BYTE = 0x01;
    constexpr char SETUP_CONTROL_BYTE = 0x03;

    void CA_02m::Execute() {
        if (i2c_fd < 0) {
            return;
        }

        char mask = ALL_OFF_BYTE;
        
        for (auto& [name, _]: chan) {
            if (chan[name]->OutVar.Get<bool>()) {
                if (name == "RED_LED") {
                    mask = mask & ~(1 << 0);
                }
                if (name == "BUZZER") {
                    mask = mask & ~(1 << 2);
                }
                if (name == "BLUE_LED") {
                    mask = mask & ~(1 << 3);
                }
                if (name == "DOUT") {
                    mask = mask & ~(1 << 1);
                }
            }

            if (name == "USB_POWER") {
                try {
                    if (chan[name]->OutVar.Get<bool>()) {
                        usb_helper.On();
                    } else {
                        usb_helper.Off();
                    }
                } catch (std::exception& ex) {
                    SetFaultState(true, ex.what());
                }
            }
        }


        if (mask != last_mask) {
            last_mask = mask;
            char buf[] = {LOAD_CONTROL_BYTE, mask};
            if (i2c_send(i2c_fd, buf, 2) < 0) {
                SetFaultState(true, "не удалось отправить данные через i2c");
            }
        }
        
        SetFaultState(false);
        
    }
    api::ScadaChannel* CA_02m::Create(const vm::Channel* channel) {
        auto name = channel->getTranslitedName();
        api::ScadaChannel* ch = new api::ScadaChannel();
        ch->BaseInit(channel);
        Register(ch);
        chan[std::string(name)] = ch;
        return ch;
    }
    CA_02m::CA_02m() {
        i2c_fd = i2c_init(FILENAME,DEVICE_ADDR);
        if (i2c_fd < 0) {
            SetFaultState(true, "не удалось инициализировать i2c");
            return;
        }
        char setup_buf[] = {SETUP_CONTROL_BYTE, 0x00};
        if (i2c_send(i2c_fd, setup_buf, 2) < 0) {
            SetFaultState(true, "не удалось отправить данные через i2c");
            return;
        }
        char blackout_buf[] = {LOAD_CONTROL_BYTE, ALL_OFF_BYTE};
        if (i2c_send(i2c_fd, blackout_buf, 2) < 0) {
            SetFaultState(true, "не удалось отправить данные через i2c");
            return;
        }
        usb_helper.Off();
        last_mask = ALL_OFF_BYTE;
    }
    CA_02m::~CA_02m() {
        i2c_terminate(i2c_fd);
    }
}} // namespace mplc::cyntron


MPLC_PROTOCOL_TYPE(CA_02m, mplc::cyntron::CA_02m);


