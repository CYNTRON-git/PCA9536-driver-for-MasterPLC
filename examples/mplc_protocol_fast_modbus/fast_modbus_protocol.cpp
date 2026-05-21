#include "fast_modbus_protocol.h"
#include <mplc/vm/node_typese.h>
#include <cstring>
#include <algorithm>

// ============================================================
//  Init — called once after all modules/channels are created
// ============================================================

void FastModbusProtocol::Init()
{
    // Open serial port
    bool ok = m_transport.open(PortName,
                               static_cast<int>(BaudRate),
                               static_cast<int>(Parity),
                               static_cast<int>(StopBits),
                               static_cast<int>(DataBits),
                               static_cast<int>(ResponseTimeoutMs));
    SetFaultState(!ok, ok ? "" : "Cannot open serial port: " + PortName);

    if (!ok) return;
    m_initialized = true;

    // Optionally scan bus to detect serial numbers
    if (AutoScan) {
        scan_bus();
    }

    // Send initial priority config to all devices
    sync_priorities();
}

// ============================================================
//  Execute — called every TaskPeriod() ms
// ============================================================

void FastModbusProtocol::Execute()
{
    if (!m_initialized || !m_transport.is_open()) {
        // Attempt re-open if port was lost
        bool ok = m_transport.open(PortName, BaudRate, Parity, StopBits, DataBits,
                                   ResponseTimeoutMs);
        if (!ok) {
            SetFaultState(true, "Serial port not available: " + PortName);
            return;
        }
        m_initialized = true;
        sync_priorities();
    }

    auto now = clock::now();
    auto* provider = LuaProvider();

    // Re-send priority config for any channel not yet synced (e.g., after reboot event)
    auto since_prio = std::chrono::duration_cast<std::chrono::seconds>(
                          now - m_last_prio_sync).count();
    if (since_prio >= 30) {
        for (auto* mod : m_modules) {
            if (mod->needs_priority_sync()) {
                sync_priorities();
                break;
            }
        }
    }

    // Event poll cycle
    auto since_poll = std::chrono::duration_cast<std::chrono::milliseconds>(
                          now - m_last_event_poll).count();
    if (EnableFastModbus && static_cast<uint32_t>(since_poll) >= static_cast<uint32_t>(EventPollIntervalMs)) {
        poll_events();
        m_last_event_poll = clock::now();
    }

    // Handle writes from MS4 and flush pending values for all modules
    now = clock::now();
    for (auto* mod : m_modules) {
        if (!mod->isExecute()) continue;

        auto writes = mod->execute(provider, now);
        for (auto& cmd : writes) {
            uint8_t addr = mod->modbus_addr;
            std::vector<uint8_t> req;
            uint8_t resp[32]{};

            if (cmd.reg_type == FMB_TYPE_COIL) {
                req = fmb::build_write_coil(addr, cmd.reg_addr, cmd.value != 0);
            } else if (cmd.reg_type == FMB_TYPE_HOLDING) {
                req = fmb::build_write_reg(addr, cmd.reg_addr, cmd.value);
            } else {
                continue; // INPUT/DISCRETE are read-only
            }

            modbus_request(req, resp, sizeof(resp));
            m_transport.wait_t35();
        }
    }

    SetFaultState(false, "");
}

// ============================================================
//  Create — called for each IOModule node in MS4 project
// ============================================================

mplc::api::ScadaModule* FastModbusProtocol::Create(const mplc::vm::IOModule* module)
{
    auto* mod = new FastModbusDeviceModule();
    mod->BaseInit(module, LuaProvider());
    m_modules.push_back(mod);
    return mod;
}

// ============================================================
//  scan_bus — send 0x01/0x02 to discover devices
// ============================================================

void FastModbusProtocol::scan_bus()
{
    if (!m_transport.is_open()) return;

    // Flush any stale bytes
    m_transport.flush_rx();

    // Phase 1: broadcast scan start
    auto req = fmb::build_scan_start(0x00);
    m_transport.send(req);

    uint8_t resp[FMB_SCAN_FRAME_LEN + 4]{};
    uint32_t last_serial = 0;
    bool first = true;

    // Arbitration loop: up to 247 devices
    for (int attempt = 0; attempt < 247; ++attempt) {
        int got = m_transport.recv(resp, sizeof(resp),
                                   static_cast<uint32_t>(ResponseTimeoutMs));
        if (got < static_cast<int>(FMB_SCAN_FRAME_LEN)) break; // no more devices

        FmbScanItem item;
        if (!fmb::parse_scan_response(resp, static_cast<size_t>(got), item)) break;

        // Match serial to a module
        for (auto* mod : m_modules) {
            if (mod->serial_number == item.serial
                || (mod->modbus_addr == item.modbus_addr && mod->serial_number == 0)) {
                mod->serial_number      = item.serial;
                mod->supports_fast_modbus = true;
                break;
            }
        }

        last_serial = item.serial;
        first = false;

        // Send scan-next to continue arbitration
        m_transport.wait_t35();
        req = fmb::build_scan_next(last_serial, 0x00);
        m_transport.send(req);
    }

    m_last_scan = clock::now();
    (void)first;
}

// ============================================================
//  sync_priorities — send 0x18 to each channel with prio != DISABLED
// ============================================================

void FastModbusProtocol::sync_priorities()
{
    for (auto* mod : m_modules) {
        for (auto* ch : mod->channels_all) {
            if (!send_event_config(mod, ch)) break; // bus error, stop for now
        }
        mod->mark_priority_synced();
    }
    m_last_prio_sync = clock::now();
}

bool FastModbusProtocol::send_event_config(FastModbusDeviceModule* mod,
                                             FastModbusRegChannel* ch)
{
    auto req = fmb::build_event_config(mod->modbus_addr,
                                        ch->reg_type,
                                        ch->reg_addr,
                                        1,        // count = 1 register
                                        ch->prio);
    uint8_t resp[16]{};
    int got = modbus_request(req, resp, sizeof(resp));
    m_transport.wait_t35();
    // ACK is [slave] 46 18 [crc L] [crc H] — 5 bytes
    return (got >= 5)
           && resp[1] == FMB_FUNC
           && resp[2] == FMB_SUB_EVT_CONFIG
           && FmbTransport::check_crc(resp, static_cast<size_t>(got));
}

// ============================================================
//  poll_events — one event request/response cycle
// ============================================================

void FastModbusProtocol::poll_events()
{
    auto* provider = LuaProvider();
    auto now       = clock::now();

    // Build event request (confirm last slave's event packet if we have one)
    auto req = fmb::build_event_request(0x01,      // min_slave_id: start from 1
                                         0xFF,      // max_data_len: accept any size
                                         m_confirm_slave,
                                         m_confirm_flag);
    m_transport.flush_rx();
    if (!m_transport.send(req)) return;

    uint8_t resp[256]{};
    int got = m_transport.recv(resp, sizeof(resp),
                               static_cast<uint32_t>(ResponseTimeoutMs));
    if (got < 5) {
        // Timeout or garbage — clear confirm to start fresh
        m_confirm_slave = 0;
        m_confirm_flag  = 0;
        return;
    }

    std::vector<FmbEvent> events;
    uint8_t new_confirm_slave = 0;
    uint8_t new_confirm_flag  = 0;

    int rc = fmb::parse_event_response(resp, static_cast<size_t>(got),
                                        events, new_confirm_slave, new_confirm_flag);
    if (rc < 0) {
        // Bad frame
        m_confirm_slave = 0;
        m_confirm_flag  = 0;
        return;
    }

    // Update confirm state for next poll
    m_confirm_slave = new_confirm_slave;
    m_confirm_flag  = new_confirm_flag;

    if (rc == 0) return; // FMB_SUB_EVT_NONE — bus quiet

    // Dispatch events to matching modules/channels
    for (auto& ev : events) {
        for (auto* mod : m_modules) {
            if (mod->modbus_addr != ev.slave_id) continue;

            // Handle REBOOT event: schedule priority re-sync for this device
            if (ev.type == FMB_TYPE_REBOOT) {
                mod->mark_priority_synced(); // reset so it syncs next cycle
                for (auto* ch : mod->channels_all) ch->prio_synced = false;
                break;
            }

            mod->dispatch_event(ev, provider, now);
            break;
        }
    }
}

// ============================================================
//  modbus_request — send frame + receive response
// ============================================================

int FastModbusProtocol::modbus_request(const std::vector<uint8_t>& req,
                                        uint8_t* resp_buf, size_t resp_max)
{
    m_transport.flush_rx();
    if (!m_transport.send(req)) return -1;
    int got = m_transport.recv(resp_buf, resp_max,
                               static_cast<uint32_t>(ResponseTimeoutMs));
    return got;
}

// ============================================================
//  fallback_poll — cyclic read when Fast Modbus not available
// ============================================================

void FastModbusProtocol::fallback_poll(FastModbusDeviceModule* mod)
{
    auto* provider = LuaProvider();
    auto now       = clock::now();

    // Group channels by reg_type and read contiguous blocks
    // For simplicity: read each channel individually (optimization possible later)
    for (auto* ch : mod->channels_all) {
        uint8_t fc = 0;
        switch (ch->reg_type) {
            case FMB_TYPE_COIL:     fc = MB_FC_READ_COILS;    break;
            case FMB_TYPE_DISCRETE: fc = MB_FC_READ_DISCRETE;  break;
            case FMB_TYPE_HOLDING:  fc = MB_FC_READ_HOLDING;   break;
            case FMB_TYPE_INPUT:    fc = MB_FC_READ_INPUT;     break;
            default: continue;
        }

        auto req = fmb::build_read_regs(mod->modbus_addr, fc, ch->reg_addr, 1);
        uint8_t resp[32]{};
        int got = modbus_request(req, resp, sizeof(resp));
        m_transport.wait_t35();

        if (got < 5) continue;
        auto vals = fmb::parse_read_response(resp, static_cast<size_t>(got));
        if (vals.empty()) continue;

        ch->on_event(static_cast<double>(vals[0]), now, provider);
    }
}
