#include "fast_modbus_module.h"
#include "fmb_frames.h"
#include <mplc/vm/node_typese.h>

// ---- Init: read device-level settings from MS4 project tree ----

void FastModbusDeviceModule::Init(const mplc::vm::IOModule* modl)
{
    {
        int v = 1;
        modl->get("ModbusAddress").GetInt(v);
        if (v < 1 || v > 247) v = 1;
        modbus_addr = static_cast<uint8_t>(v);
    }
    {
        int v = 0;
        modl->get("SerialNumber").GetInt(v);
        serial_number = static_cast<uint32_t>(v);
    }
    {
        bool b = false;
        modl->get("UseSerial").GetBool(b);
        use_serial = b;
    }
    display_name = std::string(modl->name.utf8());
}

// ---- Channel creation ----

mplc::api::ScadaChannel* FastModbusDeviceModule::Create(const mplc::vm::Channel* channel,
                                                          LuaDataProvider* provider)
{
    auto* ch = new FastModbusRegChannel();
    // BaseInit sets up InVar/OutVar and calls ch->Init(channel)
    ch->BaseInit(channel, provider);

    // Register channel in lookup table
    uint32_t key = fmb_reg_key(ch->reg_type, ch->reg_addr);
    channel_by_reg[key] = ch;
    channels_all.push_back(ch);

    return ch;
}

// ---- Event dispatch ----

void FastModbusDeviceModule::dispatch_event(const FmbEvent& ev, LuaDataProvider* provider,
                                             std::chrono::steady_clock::time_point now)
{
    uint32_t key = fmb_reg_key(ev.type, ev.reg_addr);
    auto it = channel_by_reg.find(key);
    if (it == channel_by_reg.end()) return;

    double raw = fmb::fmb_event_to_double(ev);
    it->second->on_event(raw, now, provider);
}

// ---- Execute per cycle ----

std::vector<FmbWriteCmd> FastModbusDeviceModule::execute(
    LuaDataProvider* provider, std::chrono::steady_clock::time_point now)
{
    std::vector<FmbWriteCmd> writes;

    for (auto* ch : channels_all) {
        // Flush pending (min_interval timer)
        ch->flush_pending(now, provider);

        // Collect write commands from OutVar (MS4 → device)
        if (ch->OutVar) {
            OpcUa_VariantHlp val;
            if (OpcUa_IsGood(ch->ReadVariant(provider, val))) {
                int v = 0;
                if (OpcUa_IsGood(val.GetInt(v))) {
                    FmbWriteCmd cmd;
                    cmd.reg_type = ch->reg_type;
                    cmd.reg_addr = ch->reg_addr;
                    cmd.value    = static_cast<uint16_t>(v);
                    writes.push_back(cmd);
                }
            }
        }
    }
    return writes;
}

// ---- Priority sync helpers ----

bool FastModbusDeviceModule::needs_priority_sync() const
{
    for (auto* ch : channels_all) {
        if (!ch->prio_synced) return true;
    }
    return false;
}

void FastModbusDeviceModule::mark_priority_synced()
{
    for (auto* ch : channels_all)
        ch->prio_synced = true;
}
