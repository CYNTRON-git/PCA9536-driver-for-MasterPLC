#pragma once
#include <mplc/api.h>
#include "fmb_transport.h"
#include "fmb_frames.h"
#include "fast_modbus_module.h"
#include <vector>
#include <chrono>

// Main protocol class: owns the RS-485 bus, manages scan + event polling.
// One instance per serial port (protocol node in MS4).
class FastModbusProtocol : public mplc::api::ScadaProtocol {
public:
    MPLC_OBJECT(FastModbusProtocol);

    // ---- Configurable properties (set from MS4 project tree) ----
    STRING PortName{"/dev/ttyUSB0"};
    INT    BaudRate{9600};
    INT    Parity{0};           // 0=None, 1=Even, 2=Odd
    INT    StopBits{1};
    INT    DataBits{8};
    INT    ResponseTimeoutMs{200};
    INT    InterFrameDelayMs{5};
    INT    EventPollIntervalMs{50};  // Poll events every N ms
    BOOL   EnableFastModbus{true};   // If false: fall back to cyclic Modbus RTU only
    BOOL   AutoScan{false};          // Re-scan bus on Init (discover device serial numbers)

    // ---- ScadaProtocol interface ----
    void Init() override;
    void Execute() override;
    mplc::api::ScadaModule* Create(const mplc::vm::IOModule* module) override;

private:
    FmbTransport m_transport;

    std::vector<FastModbusDeviceModule*> m_modules;

    // ---- Event polling state ----
    uint8_t m_confirm_slave{0};  // slave_id from last 0x11 to confirm
    uint8_t m_confirm_flag{0};   // flag from last 0x11 to confirm

    using clock = std::chrono::steady_clock;
    clock::time_point m_last_event_poll{};
    clock::time_point m_last_prio_sync{};
    clock::time_point m_last_scan{};

    bool m_initialized{false};

    // ---- Internal methods ----
    void scan_bus();
    void sync_priorities();
    void poll_events();

    // Send a single standard Modbus request and receive response.
    // Returns byte count received, <=0 on error.
    int modbus_request(const std::vector<uint8_t>& req, uint8_t* resp_buf, size_t resp_max);

    // Send one 0x18 priority config command for a channel; returns true on success.
    bool send_event_config(FastModbusDeviceModule* mod, FastModbusRegChannel* ch);

    // Perform a fallback cyclic poll for a module (reads all channels via standard RTU).
    void fallback_poll(FastModbusDeviceModule* mod);
};
