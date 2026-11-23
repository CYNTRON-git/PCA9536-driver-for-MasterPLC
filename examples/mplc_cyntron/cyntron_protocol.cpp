#include "cyntron_protocol.h"
#include "sys_info_collector.h"
namespace mplc { namespace cyntron {
    api::ScadaChannel* CyntronProtocol::Create(const vm::Channel* channel) {
        auto name = channel->getTranslitedName();
        api::ScadaChannel* ch = new api::ScadaChannel();
        ch->BaseInit(channel);
        Register(ch);
        chan[std::string(name)] = ch;
        return ch;
    }

    

    void CyntronProtocol::Init() {}
    void CyntronProtocol::Execute() {
        ReusableSysInfo rsi;
        for (auto const& [name, ch]: chan) {
            try {
                if (name == "CPU_TEMP")
                    ch->InVar.Update(get_cpu_temp());
                if (name == "CPU_LOAD")
                    ch->InVar.Update(static_cast<int16_t>(std::round(GetCPULoad(cpu_load_state.get()))));
                if (name == "RAM_PERCENT")
                    ch->InVar.Update(get_ram_percent(rsi));
                if (name == "DISK_PERCENT")
                    ch->InVar.Update(get_disk_percent());
                if (name == "UPTIME")
                    ch->InVar.Update(get_uptime(rsi));
                if (name == "SYS_TIME")
                    ch->InVar.Update(get_sys_time());
                if (name == "RTC_TIME")
                    ch->InVar.Update(get_rtc_time());
                if (name == "MODEM_STATUS")
                    ch->InVar.Update(get_modem_status(rsi));
                if (name == "MODEM_IFACE")
                    ch->InVar.Update(get_modem_iface(rsi));
                if (name == "USB_FLASH_DEV")
                    ch->InVar.Update(get_usb_flash_dev(rsi));
                if (name == "USB_FLASH_SIZE")
                    ch->InVar.Update(get_usb_flash_size(rsi));
                if (name == "USB_FLASH_USED")
                    ch->InVar.Update(get_usb_flash_used(rsi));
                if (name == "USB_FLASH_PERC")
                    ch->InVar.Update(get_usb_flash_perc(rsi));
                if (name == "SD_CARD_DEV")
                    ch->InVar.Update(get_sd_card_dev(rsi));
                if (name == "SD_CARD_SIZE")
                    ch->InVar.Update(get_sd_card_size(rsi));
                if (name == "SD_CARD_USED")
                    ch->InVar.Update(get_sd_card_used(rsi));
                if (name == "SD_CARD_PERC")
                    ch->InVar.Update(get_sd_card_perc(rsi));
            } catch (std::exception& ex) {
                SetFaultState(true, ex.what());
            }

        }
    }
}} // namespace mplc::cyntron

//MPLC_DECLARE_PROPERTIES(mplc::cyntron::CyntronProtocol){};

MPLC_PROTOCOL_TYPE(CyntronProtocol, mplc::cyntron::CyntronProtocol);
