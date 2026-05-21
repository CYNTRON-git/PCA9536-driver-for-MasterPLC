#pragma once
#include <mplc/api.h>
#include "fmb_defs.h"
#include "fast_modbus_channel.h"
#include <vector>
#include <unordered_map>
#include <chrono>
#include <string>

// Pending write-to-slave command collected from a channel's OutVar.
struct FmbWriteCmd {
    uint8_t  reg_type;
    uint16_t reg_addr;
    uint16_t value;
};

// Represents one physical Modbus device (slave) on the RS-485 bus.
// One FastModbusDeviceModule per device in the MS4 project tree.
class FastModbusDeviceModule : public mplc::api::ScadaModule {
public:
    // ---- Configuration (read from vm::IOModule::settings in Init()) ----
    uint8_t  modbus_addr{1};         // Modbus RTU address (1..247)
    uint32_t serial_number{0};       // Fast Modbus serial number (0 = not known)
    bool     use_serial{false};      // Address by serial number instead of MB addr
    bool     supports_fast_modbus{false}; // Confirmed during scan

    // ---- Channel lookup ----
    // Key: fmb_reg_key(type, reg_addr) → channel*
    std::unordered_map<uint32_t, FastModbusRegChannel*> channel_by_reg;
    std::vector<FastModbusRegChannel*> channels_all;

    // ---- Interface called by FastModbusProtocol ----

    // Dispatch an incoming event to the matching channel.
    void dispatch_event(const FmbEvent& ev, LuaDataProvider* provider,
                        std::chrono::steady_clock::time_point now);

    // Flush all pending values and collect write commands from OutVars.
    // Returns list of pending write commands.
    std::vector<FmbWriteCmd> execute(LuaDataProvider* provider,
                                      std::chrono::steady_clock::time_point now);

    // Returns true if any channel has prio_synced == false.
    bool needs_priority_sync() const;

    // Mark all channels as priority-synced (called after 0x18 commands sent).
    void mark_priority_synced();

    // ScadaModule interface
    mplc::api::ScadaChannel* Create(const mplc::vm::Channel* channel,
                                     LuaDataProvider* provider) override;

    std::string display_name; // for logging

protected:
    void Init(const mplc::vm::IOModule* modl) override;
};
