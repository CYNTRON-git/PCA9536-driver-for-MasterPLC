#include "fmb_frames.h"
#include "fmb_transport.h"
#include <cstring>

namespace fmb {

// ---- helpers ----
static void push_u16be(std::vector<uint8_t>& v, uint16_t val) {
    v.push_back(static_cast<uint8_t>(val >> 8));
    v.push_back(static_cast<uint8_t>(val & 0xFF));
}
static void push_u32be(std::vector<uint8_t>& v, uint32_t val) {
    v.push_back(static_cast<uint8_t>((val >> 24) & 0xFF));
    v.push_back(static_cast<uint8_t>((val >> 16) & 0xFF));
    v.push_back(static_cast<uint8_t>((val >>  8) & 0xFF));
    v.push_back(static_cast<uint8_t>( val        & 0xFF));
}
static uint16_t read_u16be(const uint8_t* p) {
    return static_cast<uint16_t>((p[0] << 8) | p[1]);
}
static uint32_t read_u32be(const uint8_t* p) {
    return (static_cast<uint32_t>(p[0]) << 24)
         | (static_cast<uint32_t>(p[1]) << 16)
         | (static_cast<uint32_t>(p[2]) <<  8)
         |  static_cast<uint32_t>(p[3]);
}

// ============================================================
//  Standard Modbus RTU builders
// ============================================================

std::vector<uint8_t> build_read_regs(uint8_t addr, uint8_t fc, uint16_t reg, uint16_t count)
{
    std::vector<uint8_t> f;
    f.reserve(8);
    f.push_back(addr);
    f.push_back(fc);
    push_u16be(f, reg);
    push_u16be(f, count);
    FmbTransport::append_crc(f);
    return f;
}

std::vector<uint8_t> build_write_reg(uint8_t addr, uint16_t reg, uint16_t value)
{
    std::vector<uint8_t> f;
    f.push_back(addr);
    f.push_back(MB_FC_WRITE_REGISTER);
    push_u16be(f, reg);
    push_u16be(f, value);
    FmbTransport::append_crc(f);
    return f;
}

std::vector<uint8_t> build_write_regs(uint8_t addr, uint16_t reg,
                                       const uint16_t* values, uint8_t count)
{
    std::vector<uint8_t> f;
    f.push_back(addr);
    f.push_back(MB_FC_WRITE_REGS);
    push_u16be(f, reg);
    push_u16be(f, count);
    f.push_back(static_cast<uint8_t>(count * 2));
    for (uint8_t i = 0; i < count; ++i) push_u16be(f, values[i]);
    FmbTransport::append_crc(f);
    return f;
}

std::vector<uint8_t> build_write_coil(uint8_t addr, uint16_t reg, bool value)
{
    std::vector<uint8_t> f;
    f.push_back(addr);
    f.push_back(MB_FC_WRITE_COIL);
    push_u16be(f, reg);
    push_u16be(f, value ? 0xFF00u : 0x0000u);
    FmbTransport::append_crc(f);
    return f;
}

// ============================================================
//  Standard Modbus RTU parsers
// ============================================================

std::vector<uint16_t> parse_read_response(const uint8_t* frame, size_t len)
{
    std::vector<uint16_t> result;
    // Minimum: addr + fc + byte_count + ≥1 byte + crc(2) = 5 bytes
    if (len < 5) return result;
    if (!FmbTransport::check_crc(frame, len)) return result;
    uint8_t fc = frame[1];
    if (fc & 0x80) return result; // exception

    uint8_t byte_count = frame[2];
    if (static_cast<size_t>(byte_count + 5) > len) return result;

    const uint8_t* data = frame + 3;
    if (fc == MB_FC_READ_COILS || fc == MB_FC_READ_DISCRETE) {
        // Each byte holds 8 coil values (LSB first)
        for (uint8_t b = 0; b < byte_count; ++b) {
            for (int bit = 0; bit < 8; ++bit)
                result.push_back((data[b] >> bit) & 1);
        }
    } else {
        // FC03/04: pairs of bytes BE
        for (uint8_t i = 0; i + 1 < byte_count; i += 2)
            result.push_back(read_u16be(data + i));
    }
    return result;
}

// ============================================================
//  Fast Modbus (0x46) builders
// ============================================================

std::vector<uint8_t> build_scan_start(uint8_t priority_bits)
{
    // FD 46 01 [prio_bits] + CRC  — 6 bytes total
    std::vector<uint8_t> f;
    f.push_back(FMB_BROADCAST_ADDR);
    f.push_back(FMB_FUNC);
    f.push_back(FMB_SUB_SCAN_START);
    f.push_back(priority_bits);
    FmbTransport::append_crc(f);
    return f;
}

std::vector<uint8_t> build_scan_next(uint32_t last_serial, uint8_t priority_bits)
{
    // FD 46 02 [serial BE 4B] [prio_bits] + CRC  — 10 bytes total
    std::vector<uint8_t> f;
    f.push_back(FMB_BROADCAST_ADDR);
    f.push_back(FMB_FUNC);
    f.push_back(FMB_SUB_SCAN_NEXT);
    push_u32be(f, last_serial);
    f.push_back(priority_bits);
    FmbTransport::append_crc(f);
    return f;
}

std::vector<uint8_t> build_event_request(uint8_t min_slave_id, uint8_t max_data_len,
                                          uint8_t confirm_slave, uint8_t confirm_flag)
{
    // FD 46 10 [min_slave] [max_data_len] [confirm_slave] [confirm_flag] + CRC — 9 bytes total
    std::vector<uint8_t> f;
    f.push_back(FMB_BROADCAST_ADDR);
    f.push_back(FMB_FUNC);
    f.push_back(FMB_SUB_EVT_REQUEST);
    f.push_back(min_slave_id);
    f.push_back(max_data_len);
    f.push_back(confirm_slave);
    f.push_back(confirm_flag);
    FmbTransport::append_crc(f);
    return f;
}

std::vector<uint8_t> build_event_config(uint8_t slave_addr, uint8_t type,
                                         uint16_t reg_addr, uint16_t count,
                                         uint8_t prio)
{
    // [addr] 46 18 05 [type] [reg H] [reg L] [count H] [count L] [prio] + CRC
    // len byte = 0x05 (5 data bytes after len)
    std::vector<uint8_t> f;
    f.push_back(slave_addr);
    f.push_back(FMB_FUNC);
    f.push_back(FMB_SUB_EVT_CONFIG);
    f.push_back(0x05);           // length of following data
    f.push_back(type);
    push_u16be(f, reg_addr);
    push_u16be(f, count);
    f.push_back(prio);
    FmbTransport::append_crc(f);
    return f;
}

std::vector<uint8_t> build_by_serial_request(uint32_t serial,
                                               const std::vector<uint8_t>& inner_pdu)
{
    // FD 46 08 [serial BE 4B] [inner_pdu...] + CRC
    std::vector<uint8_t> f;
    f.push_back(FMB_BROADCAST_ADDR);
    f.push_back(FMB_FUNC);
    f.push_back(FMB_SUB_BY_SER_REQ);
    push_u32be(f, serial);
    f.insert(f.end(), inner_pdu.begin(), inner_pdu.end());
    FmbTransport::append_crc(f);
    return f;
}

// ============================================================
//  Fast Modbus parsers
// ============================================================

bool parse_scan_response(const uint8_t* frame, size_t len, FmbScanItem& out)
{
    // Expected: [slave_id] 46 03 [serial BE 4B] [mb_addr] [unused] [crc L] [crc H]
    // Total: 10 bytes
    if (len < FMB_SCAN_FRAME_LEN) return false;
    if (!FmbTransport::check_crc(frame, len)) return false;
    if (frame[1] != FMB_FUNC && frame[1] != FMB_FUNC_LEGACY) return false;
    if (frame[2] != FMB_SUB_SCAN_RSP) return false;

    out.serial      = read_u32be(frame + 3);
    out.modbus_addr = frame[7];
    return true;
}

int parse_event_response(const uint8_t* frame, size_t len,
                          std::vector<FmbEvent>& events,
                          uint8_t& out_confirm_slave,
                          uint8_t& out_confirm_flag)
{
    if (len < 5) return -1;
    if (!FmbTransport::check_crc(frame, len)) return -1;
    if (frame[1] != FMB_FUNC && frame[1] != FMB_FUNC_LEGACY) return -1;

    uint8_t sub = frame[2];

    if (sub == FMB_SUB_EVT_NONE) {
        // FD 46 12 + CRC — no events
        return 0;
    }

    if (sub != FMB_SUB_EVT_TRANSMIT) return -1;

    // [slave_id] 46 11 [flag] [event_count] [data_len] [events...] [crc L] [crc H]
    // Minimum length: 6 (addr + func + sub + flag + count + data_len) + 2 CRC = 8
    if (len < 8) return -1;

    uint8_t slave_id   = frame[0];
    uint8_t flag       = frame[3];
    uint8_t evt_count  = frame[4];
    uint8_t data_len   = frame[5];

    out_confirm_slave = slave_id;
    out_confirm_flag  = flag;

    if (evt_count == 0) return 1; // acknowledged, no actual events

    const uint8_t* evt_ptr = frame + 6;
    const uint8_t* evt_end = frame + 6 + data_len; // may not reach end before CRC
    // Guard: don't read past CRC
    if (evt_end > frame + len - 2) evt_end = frame + len - 2;

    events.reserve(evt_count);

    for (uint8_t i = 0; i < evt_count && evt_ptr < evt_end; ++i) {
        // Each event: [plen] [type] [reg H] [reg L] [payload (plen-3 bytes)]
        // plen = 1+2+(payload_bytes) = type(1) + reg(2) + payload
        if (evt_ptr + 1 > evt_end) break;
        uint8_t plen = evt_ptr[0]; // total bytes: type + reg_h + reg_l + payload
        evt_ptr++;

        if (plen < 3) break; // malformed
        if (evt_ptr + plen > evt_end) break;

        FmbEvent ev{};
        ev.slave_id   = slave_id;
        ev.type       = evt_ptr[0];
        ev.reg_addr   = read_u16be(evt_ptr + 1);
        ev.payload_len = static_cast<uint8_t>(plen - 3);
        if (ev.payload_len > 2) ev.payload_len = 2; // guard
        memcpy(ev.payload, evt_ptr + 3, ev.payload_len);
        events.push_back(ev);

        evt_ptr += plen;
    }

    return 1;
}

double fmb_event_to_double(const FmbEvent& ev)
{
    switch (ev.payload_len) {
        case 0: return 0.0; // REBOOT
        case 1: return static_cast<double>(ev.payload[0]); // COIL / DISCRETE
        case 2: { // HOLDING / INPUT: big-endian uint16, interpret as int16 for signed
            uint16_t raw = (static_cast<uint16_t>(ev.payload[0]) << 8) | ev.payload[1];
            return static_cast<double>(raw);
        }
        default: return 0.0;
    }
}

} // namespace fmb
