#pragma once
#include "fmb_defs.h"
#include <vector>
#include <cstdint>

// ---- Frame builder/parser functions ----
// All builders return a frame WITHOUT CRC (caller appends via FmbTransport::append_crc).
// All parsers expect the full frame INCLUDING CRC and validate it internally.

namespace fmb {

// ---- Standard Modbus RTU builders ----

std::vector<uint8_t> build_read_regs(uint8_t addr, uint8_t fc, uint16_t reg, uint16_t count);
std::vector<uint8_t> build_write_reg(uint8_t addr, uint16_t reg, uint16_t value);
std::vector<uint8_t> build_write_regs(uint8_t addr, uint16_t reg,
                                       const uint16_t* values, uint8_t count);
std::vector<uint8_t> build_write_coil(uint8_t addr, uint16_t reg, bool value);

// ---- Standard Modbus RTU parsers ----

// Parse FC01/02/03/04 read response. Returns register/coil values or empty on error.
// For FC01/02 (coils/discrete): each element is 0 or 1.
// For FC03/04 (registers): each element is a uint16 value.
std::vector<uint16_t> parse_read_response(const uint8_t* frame, size_t len);

// ---- Fast Modbus (0x46) builders ----

// Scan start — broadcast, addr=0xFD, sub=0x01 [+priority_bits: 0x00]
std::vector<uint8_t> build_scan_start(uint8_t priority_bits = 0x00);

// Scan next — broadcast, addr=0xFD, sub=0x02 [+last_seen_serial BE 4B + priority_bits]
std::vector<uint8_t> build_scan_next(uint32_t last_serial, uint8_t priority_bits = 0x00);

// Event request — broadcast, addr=0xFD, sub=0x10
// confirm_slave/flag: 0,0 if no prior event to confirm
std::vector<uint8_t> build_event_request(uint8_t min_slave_id = 0x01,
                                          uint8_t max_data_len  = 0xFF,
                                          uint8_t confirm_slave = 0x00,
                                          uint8_t confirm_flag  = 0x00);

// Event priority config — unicast to slave, sub=0x18
// type: FMB_TYPE_*; prio: FMB_PRIO_*
std::vector<uint8_t> build_event_config(uint8_t slave_addr, uint8_t type,
                                         uint16_t reg_addr, uint16_t count,
                                         uint8_t prio);

// Serial-addressed inner PDU wrapper (for sending standard Modbus request via serial)
std::vector<uint8_t> build_by_serial_request(uint32_t serial,
                                               const std::vector<uint8_t>& inner_pdu);

// ---- Fast Modbus parsers ----

// Parse scan response (FMB_SUB_SCAN_RSP). Returns {serial, modbus_addr}.
// Returns false on bad frame or wrong subcommand.
bool parse_scan_response(const uint8_t* frame, size_t len, FmbScanItem& out);

// Parse event response frame.
// Returns: 1 if events found (events vector populated), 0 if FMB_SUB_EVT_NONE, -1 on error.
// confirm_slave/confirm_flag are set from the 0x11 response for use in next 0x10 request.
int parse_event_response(const uint8_t* frame, size_t len,
                          std::vector<FmbEvent>& events,
                          uint8_t& out_confirm_slave,
                          uint8_t& out_confirm_flag);

// Convert raw event payload bytes to double value
double fmb_event_to_double(const FmbEvent& ev);

} // namespace fmb
