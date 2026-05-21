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
    uint16_t value;     // raw register value (inverse-scaled from physical)
};

// Represents one physical Modbus device (slave) on the RS-485 bus.
class FastModbusDeviceModule : public mplc::api::ScadaModule {
public:
    // ---- Configuration (read from vm::IOModule::settings in Init()) ----
    uint8_t  modbus_addr{1};              // Modbus RTU address 1..247
    uint32_t serial_number{0};            // Fast Modbus serial (0 = unknown)
    bool     use_serial{false};           // Address by serial instead of MB addr
    bool     supports_fast_modbus{false}; // Confirmed during scan

    // ---- Channel lookup ----
    std::unordered_map<uint32_t, FastModbusRegChannel*> channel_by_reg; // key: fmb_reg_key
    std::vector<FastModbusRegChannel*> channels_all;

    // ---- Called by FastModbusProtocol each Execute() cycle ----

    // Dispatch an incoming Fast Modbus event to the matching channel.
    void dispatch_event(const FmbEvent& ev, LuaDataProvider* provider,
                        std::chrono::steady_clock::time_point now);

    // Flush pending values whose min_interval has elapsed.
    void flush_pending_values(LuaDataProvider* provider,
                               std::chrono::steady_clock::time_point now);

    // Collect write commands from OutVars (MS4 → device).
    // Only returns commands for values that changed (IsNeedWrite).
    // Applies inverse scale: raw = (physical - offset) / scale.
    std::vector<FmbWriteCmd> collect_writes(LuaDataProvider* provider);

    // ---- Priority sync helpers ----
    bool needs_priority_sync() const;
    void mark_priority_synced();         // Mark all channels prio_synced=true
    void reset_prio_sync();              // Mark all channels prio_synced=false (re-sync needed)

    // ScadaModule interface
    mplc::api::ScadaChannel* Create(const mplc::vm::Channel* channel,
                                     LuaDataProvider* provider) override;

    std::string display_name;

protected:
    void Init(const mplc::vm::IOModule* modl) override;
};
