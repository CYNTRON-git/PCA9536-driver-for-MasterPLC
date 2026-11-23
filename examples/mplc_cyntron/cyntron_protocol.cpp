#include "cyntron_protocol.h"
#include "sys_info_collector.h"
namespace mplc { namespace cyntron {
namespace {
inline mplc::DateTime get_sys_time() {
    timeval tv{};
    if (gettimeofday(&tv, nullptr) < 0) throw std::runtime_error("Cannot read system clock");
    return mplc::DateTime::from_unix(tv.tv_sec);
}
inline mplc::DateTime get_rtc_time(ExternalRtc& source) {
    const rtc_time rtc = source.read();
    return mplc::DateTime(1900 + rtc.tm_year, rtc.tm_mon + 1, rtc.tm_mday,
                          rtc.tm_hour, rtc.tm_min, rtc.tm_sec);
}
}
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
        if (!isConnect() || !isExecute()) return;
        std::string error;
        ReusableSysInfo rsi;
        for (auto const& [name, ch]: chan) {
            try {
                if (name == "CPU_TEMP")
                    ch->InVar.Update(get_cpu_temp());
                if (name == "CPU_LOAD") {
                    if (!cpu_load_state) throw std::runtime_error("CPU load initialization failed");
                    const double load = GetCPULoad(cpu_load_state.get());
                    if (!std::isfinite(load) || load < 0 || load > 100) throw std::runtime_error("Invalid CPU load");
                    ch->InVar.Update(static_cast<int16_t>(std::round(load)));
                }
                if (name == "RAM_PERCENT")
                    ch->InVar.Update(get_ram_percent());
                if (name == "DISK_PERCENT")
                    ch->InVar.Update(filesystemData("/").percent);
                if (name == "UPTIME")
                    ch->InVar.Update(get_uptime());
                if (name == "SYS_TIME")
                    ch->InVar.Update(get_sys_time());
                if (name == "RTC_TIME")
                    ch->InVar.Update(get_rtc_time(external_rtc));
                if (name == "MODEM_STATUS")
                    ch->InVar.Update(get_modem_status(rsi));
                if (name == "MODEM_IFACE")
                    ch->InVar.Update(get_modem_iface(rsi));
                if (name == "USB_FLASH_DEV")
                    ch->InVar.Update(rsi.usbFlash().mounted);
                if (name == "USB_FLASH_SIZE")
                    ch->InVar.Update(rsi.usbFlash().size);
                if (name == "USB_FLASH_USED")
                    ch->InVar.Update(rsi.usbFlash().used);
                if (name == "USB_FLASH_PERC")
                    ch->InVar.Update(rsi.usbFlash().percent);
                if (name == "SD_CARD_DEV")
                    ch->InVar.Update(rsi.sdCard().mounted);
                if (name == "SD_CARD_SIZE")
                    ch->InVar.Update(rsi.sdCard().size);
                if (name == "SD_CARD_USED")
                    ch->InVar.Update(rsi.sdCard().used);
                if (name == "SD_CARD_PERC")
                    ch->InVar.Update(rsi.sdCard().percent);
            } catch (std::exception& ex) {
                ch->InVar.SetQuality(OpcUa_BadCommunicationError);
                if (error.empty()) error = name + ": " + ex.what();
            }

        }
        SetFaultState(!error.empty(), error);
    }
}} // namespace mplc::cyntron

//MPLC_DECLARE_PROPERTIES(mplc::cyntron::CyntronProtocol){};

MPLC_PROTOCOL_TYPE(CyntronProtocol, mplc::cyntron::CyntronProtocol);
