#include "simple_test_protocol.h"
#include <cmath>
#include <cstring>
#include <vector>
#include <algorithm>
#include <limits>

// Вспомогательная функция для выполнения команд
std::string exec_command(const std::string& cmd) {
    char buffer[128];
    std::string result;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (pipe) {
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);
    }
    // Удаляем завершающие пробельные символы
    size_t endpos = result.find_last_not_of(" \n\r\t");
    if (endpos != std::string::npos) {
        result = result.substr(0, endpos+1);
    }
    return result;
}

// Вспомогательная функция преобразования в int16_t с обработкой ошибок
int16_t clamp_to_int16(double value) {
    const double min_value = static_cast<double>(std::numeric_limits<int16_t>::min());
    const double max_value = static_cast<double>(std::numeric_limits<int16_t>::max());
    value = std::clamp(value, min_value, max_value);
    return static_cast<int16_t>(value);
}

// Вспомогательная функция преобразования в int16_t с обработкой ошибок
int16_t to_int16(const std::string& value, float scale = 1.0f) {
    if (value.empty()) return 0;
    try {
        float fval = std::stof(value) * scale;
        return clamp_to_int16(std::round(fval));
    } catch (...) {
        return 0;
    }
}

// /proc/uptime отдает секунды с дробной частью; для часов работы нужен
// счетчик полных часов без округления вверх.
int16_t uptime_seconds_to_hours(const std::string& value) {
    if (value.empty()) return 0;
    try {
        double seconds = std::stod(value);
        if (seconds <= 0.0) return 0;
        return clamp_to_int16(std::floor(seconds / 3600.0));
    } catch (...) {
        return 0;
    }
}

// Функция округления до ближайшей степени двойки
int16_t round_to_power_of_two(double value_gb) {
    if (value_gb <= 0) return 0;
    if (value_gb < 1) return 1;
    
    // Таблица стандартных размеров
    static const std::vector<int16_t> sizes = {
        1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096
    };
    
    // Ищем ближайшее большее значение
    for (auto size : sizes) {
        if (size >= value_gb) {
            return size;
        }
    }
    
    // Если размер больше максимального в таблице
    return sizes.back();
}

SimpleTestProtocol::SimpleTestProtocol() {
    emulator = new Emulator();
}

SimpleTestProtocol::~SimpleTestProtocol() {
    emulator->close();
    delete emulator;
}

void SimpleTestProtocol::Init() {
    emulator->init();
    last_mask = cyntron_ca02m::kAllOffMask;
    outputs_initialized = false;
}

void SimpleTestProtocol::Execute() {
    bool blue_led = false;
    bool red_led = false;
    bool buzzer = false;
    bool dout = false;
    bool usb_power = true;
    bool have_outputs = false;

    // Кэшируем результаты для связанных команд
    static std::string cpu_temp_cache;
    static std::string cpu_load_cache;
    static std::string ram_percent_cache;
    static std::string disk_percent_cache;
    static std::string uptime_cache;
    static std::string modem_check_cache;
    static std::string usb_mount_cache;
    static std::string sd_mount_cache;

    // Флаги для проверки необходимости обновления кэша
    bool need_cpu_temp = false;
    bool need_cpu_load = false;
    bool need_ram_percent = false;
    bool need_disk_percent = false;
    bool need_uptime = false;
    bool need_modem_check = false;
    bool need_usb_mount = false;
    bool need_sd_mount = false;

    // Определяем, какие данные нужны в этом цикле
    for (auto ch : channels) {
        if (ch->InVar) {
            const std::string& path = ch->InVar->m_var->project_path();
            if (path == "CPU_TEMP") need_cpu_temp = true;
            else if (path == "CPU_LOAD") need_cpu_load = true;
            else if (path == "RAM_PERCENT") need_ram_percent = true;
            else if (path == "DISK_PERCENT") need_disk_percent = true;
            else if (path == "UPTIME") need_uptime = true;
            else if (path == "MODEM_STATUS" || path == "MODEM_IFACE") need_modem_check = true;
            else if (path.find("USB_FLASH_") != std::string::npos) need_usb_mount = true;
            else if (path.find("SD_CARD_") != std::string::npos) need_sd_mount = true;
        }
    }

    // Выполняем команды только для необходимых данных
    if (need_cpu_temp) {
        cpu_temp_cache = exec_command("cat /sys/class/thermal/thermal_zone0/temp");
    }
    if (need_cpu_load) {
        cpu_load_cache = exec_command("top -b -n1 | grep \"Cpu(s)\" | awk '{print $2 + $4}'");
    }
    if (need_ram_percent) {
        ram_percent_cache = exec_command("free | grep Mem | awk '{print $3/$2 * 100.0}'");
    }
    if (need_disk_percent) {
        disk_percent_cache = exec_command("df -h / | awk 'NR==2{print $5}' | tr -d '%'");
    }
    if (need_uptime) {
        uptime_cache = exec_command("awk '{print $1}' /proc/uptime");
    }
    if (need_modem_check) {
        modem_check_cache = exec_command("ls /sys/class/net/ | grep 'usb0'");
    }
    if (need_usb_mount) {
        usb_mount_cache = exec_command("mount | grep '/media/usb'");
    }
    if (need_sd_mount) {
        sd_mount_cache = exec_command("mount | grep '/media/sdcard'");
    }

    // Обрабатываем каналы
    for (auto ch : channels) {
        if (ch->InVar) {
            const std::string& path = ch->InVar->m_var->project_path();
            
            if (path == "CPU_TEMP") {
                int16_t temp = to_int16(cpu_temp_cache, 0.001f);
                ch->Write(LuaProvider(), temp);
            }
            else if (path == "CPU_LOAD") {
                int16_t load = to_int16(cpu_load_cache);
                ch->Write(LuaProvider(), load);
            }
            else if (path == "RAM_PERCENT") {
                int16_t ram = to_int16(ram_percent_cache);
                ch->Write(LuaProvider(), ram);
            }
            else if (path == "DISK_PERCENT") {
                int16_t disk = to_int16(disk_percent_cache);
                ch->Write(LuaProvider(), disk);
            }
            else if (path == "UPTIME") {
                int16_t uptime = uptime_seconds_to_hours(uptime_cache);
                ch->Write(LuaProvider(), uptime);
            }
            else if (path == "SYS_TIME") {
                std::string time = exec_command("date \"+%Y-%m-%d %H:%M:%S\"");
                ch->Write(LuaProvider(), time);
            }
            else if (path == "RTC_TIME") {
                std::string rtc = exec_command("hwclock -r -f /dev/rtc0");
                ch->Write(LuaProvider(), rtc);
            }
            else if (path == "MODEM_STATUS") {
                std::string status = "NOT CONNECTED";
                if (modem_check_cache.find("usb0") != std::string::npos) {
                    status = exec_command("ip link show usb0 | awk '{print $9}' | head -n 1");
                }
                ch->Write(LuaProvider(), status);
            }
            else if (path == "MODEM_IFACE") {
                std::string iface = "NOT CONNECTED";
                if (modem_check_cache.find("usb0") != std::string::npos) {
                    iface = "/sys/class/net/usb0";
                    USB_MOD_CONNED = true;
                } else {
                    USB_MOD_CONNED = false;
                }
                ch->Write(LuaProvider(), iface);
            }
            else if (path == "USB_FLASH_DEV") {
                std::string dev = usb_mount_cache.empty() ? 
                                 "NOT CONNECTED" : "/media/usb";
                ch->Write(LuaProvider(), dev);
            }
            else if (path == "USB_FLASH_SIZE") {
                if (usb_mount_cache.empty()) {
                    ch->Write(LuaProvider(), static_cast<int16_t>(0));
                } else {
                    std::string size_str = exec_command("df -m /media/usb | awk 'NR==2{print $2}'");
                    double size_mb = std::atof(size_str.c_str());
                    double size_gb = size_mb / 1024.0;
                    int16_t size_rounded = round_to_power_of_two(size_gb);
                    ch->Write(LuaProvider(), size_rounded);
                }
            }
            else if (path == "USB_FLASH_USED") {
                if (usb_mount_cache.empty()) {
                    ch->Write(LuaProvider(), static_cast<int16_t>(0));
                } else {
                    std::string used = exec_command("df -m /media/usb | awk 'NR==2{print $3}'");
                    int16_t used_mb = to_int16(used);
                    ch->Write(LuaProvider(), used_mb);
                }
            }
            else if (path == "USB_FLASH_PERC") {
                if (usb_mount_cache.empty()) {
                    ch->Write(LuaProvider(), static_cast<int16_t>(0));
                } else {
                    std::string perc = exec_command("df -m /media/usb | awk 'NR==2{print $5}'");
                    int16_t perc_val = to_int16(perc);
                    ch->Write(LuaProvider(), perc_val);
                }
            }
            else if (path == "SD_CARD_DEV") {
                std::string dev = sd_mount_cache.empty() ? 
                                  "NOT CONNECTED" : "/media/sdcard";
                ch->Write(LuaProvider(), dev);
            }
            else if (path == "SD_CARD_SIZE") {
                if (sd_mount_cache.empty()) {
                    ch->Write(LuaProvider(), static_cast<int16_t>(0));
                } else {
                    std::string size_str = exec_command("df -m /media/sdcard | awk 'NR==2{print $2}'");
                    double size_mb = std::atof(size_str.c_str());
                    double size_gb = size_mb / 1024.0;
                    int16_t size_rounded = round_to_power_of_two(size_gb);
                    ch->Write(LuaProvider(), size_rounded);
                }
            }
            else if (path == "SD_CARD_USED") {
                if (sd_mount_cache.empty()) {
                    ch->Write(LuaProvider(), static_cast<int16_t>(0));
                } else {
                    std::string used = exec_command("df -m /media/sdcard | awk 'NR==2{print $3}'");
                    int16_t used_mb = to_int16(used);
                    ch->Write(LuaProvider(), used_mb);
                }
            }
            else if (path == "SD_CARD_PERC") {
                if (sd_mount_cache.empty()) {
                    ch->Write(LuaProvider(), static_cast<int16_t>(0));
                } else {
                    std::string perc = exec_command("df -m /media/sdcard | awk 'NR==2{print $5}'");
                    int16_t perc_val = to_int16(perc);
                    ch->Write(LuaProvider(), perc_val);
                }
            }
        }

        if (ch->OutVar) {
            std::string buff;
            ch->OutVar->Read(LuaProvider());
            ch->OutVar->Value.GetString(buff);
            const bool enabled = (buff == "true");
            have_outputs = true;

            if (ch->OutVar->m_var->project_path() == "BLUE_LED") {
                blue_led = enabled;
            }
            else if (ch->OutVar->m_var->project_path() == "RED_LED") {
                red_led = enabled;
            }
            else if (ch->OutVar->m_var->project_path() == "BUZZER") {
                buzzer = enabled;
            }
            else if (ch->OutVar->m_var->project_path() == "USB_POWER") {
                usb_power = enabled;
            }
            else if (ch->OutVar->m_var->project_path() == "DOUT") {
                dout = enabled;
            }
        }
    }

    if (!have_outputs) {
        return;
    }

    const uint8_t new_mask = cyntron_ca02m::build_output_mask(blue_led, red_led, buzzer, dout);

    try {
        if (!outputs_initialized || new_mask != last_mask) {
            pca9536_.writeOutputs(new_mask);
            last_mask = new_mask;
            outputs_initialized = true;
        }
        usb_power_.setEnabled(usb_power);
        SetFaultState(false, "");
    } catch (const std::exception& ex) {
        outputs_initialized = false;
        SetFaultState(true, ex.what());
    }
}

mplc::api::ScadaChannel* SimpleTestProtocol::Create(const mplc::vm::Channel* channel) {
    auto ch = new mplc::api::ScadaChannel();
    channels.push_back(ch);
    return ch;
}

MPLC_PROTOCOL_TYPE(Diagnostic, SimpleTestProtocol);