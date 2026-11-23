#include "../examples/common/ca02m_hw.h"
#include "../examples/mplc_cyntron/gpio_helper.h"
#include "../examples/mplc_cyntron/output_state.h"
#include "../examples/mplc_cyntron/sys_info_collector.h"
#include "../examples/mplc_cyntron/CA_02m.h"
#include "../examples/mplc_cyntron/cyntron_protocol.h"
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <fstream>

using namespace mplc::cyntron;
using namespace cyntron_ca02m;
namespace {
int bus = -1, chip = -1, line = -1;
int bus_opens = 0, bus_closes = 0, interrupts = 0, short_writes = 0;
uint8_t selected = 0, registers[4] = {0, 15, 0, 15};
int level = -1;
bool reject_address = false, rtc_missing = true;
int rtc_fd = -1;
int rtc_bus = -1, rtc_address = 0, rtc_reads = 0, rtc_node_reads = 0;
bool rtc0_only = false;
int rtc0_opens = 0;
bool rtc_bus_busy = false;
uint8_t rtc_status = 0;
std::array<uint8_t,7> rtc_data{0x30,0x15,0x10,0x02,0x23,0x11,0x26};
std::string root, lockfile;
}
extern "C" {
int __real_open(const char*, int, ...);
int __real_close(int);
ssize_t __real_read(int, void*, size_t);
ssize_t __real_write(int, const void*, size_t);
int __wrap_open(const char* path, int flags, ...) {
    if (!strcmp(path, "/dev/rtc0") || !strcmp(path, "/dev/rtc1")) {
        if (!strcmp(path, "/dev/rtc0")) ++rtc0_opens;
        if (rtc_missing || (rtc0_only && !strcmp(path, "/dev/rtc1"))) { errno = ENOENT; return -1; }
        return rtc_fd = __real_open("/dev/null", O_RDWR);
    }
    if (!strcmp(path, "/dev/i2c-1")) return rtc_bus = __real_open("/dev/null", flags);
    if (!strcmp(path, kI2cPath)) {
        ++bus_opens;
        return bus = __real_open("/dev/null", O_RDWR);
    }
    if (!strcmp(path, kGpioChipPath)) return chip = __real_open("/dev/null", O_RDWR);
    mode_t mode = 0;
    if (flags & O_CREAT) {
        va_list ap;
        va_start(ap, flags);
        mode = va_arg(ap, int);
        va_end(ap);
    }
    return __real_open(!strcmp(path, kLockFile) ? lockfile.c_str() : path, flags, mode);
}
int __wrap_close(int fd) {
    if (fd == bus) { bus = -1; ++bus_closes; }
    if (fd == chip) chip = -1;
    if (fd == line) line = -1;
    if (fd == rtc_fd) rtc_fd = -1;
    if (fd == rtc_bus) rtc_bus = -1;
    return __real_close(fd);
}
int __wrap_ioctl(int fd, unsigned long op, ...) {
    if (fd == bus) {
        if (op == I2C_SLAVE && reject_address) { errno = ENXIO; return -1; }
        return 0;
    }
    va_list args;
    va_start(args, op);
    if (fd == rtc_bus) {
        if (op == I2C_SLAVE) {
            rtc_address = int(va_arg(args, unsigned long));
            va_end(args);
            if (rtc_bus_busy) { errno = EBUSY; return -1; }
            return 0;
        }
        assert(op == I2C_RDWR);
        auto* request = va_arg(args, i2c_rdwr_ioctl_data*);
        va_end(args);
        assert(request->nmsgs == 2); // Four messages return spurious zeroes on the A40i.
        for (unsigned i = 0; i < request->nmsgs; i += 2) {
            auto command = request->msgs[i], data = request->msgs[i+1];
            assert(command.addr == rtc_address && command.flags == 0 && command.len == 1);
            assert(data.addr == rtc_address && data.flags == I2C_M_RD);
            if (data.len == 1) { assert(command.buf[0] == 0x0f); data.buf[0] = rtc_status; }
            else {
                assert(data.len == rtc_data.size() && command.buf[0] == (rtc_address == 0x68 ? 0x00 : 0x02));
                std::copy(rtc_data.begin(), rtc_data.end(), data.buf);
            }
        }
        ++rtc_reads; return int(request->nmsgs);
    }
    if (fd == rtc_fd && op == RTC_RD_TIME) {
        ++rtc_node_reads;
        auto rtc = va_arg(args, rtc_time*);
        rtc->tm_year = 126; rtc->tm_mon = 10; rtc->tm_mday = 23;
    } else if (fd == chip && op == GPIO_GET_LINEHANDLE_IOCTL) {
        auto req = va_arg(args, gpiohandle_request*);
        assert(req->lines == 1 && req->lineoffsets[0] == 268);
        level = req->default_values[0];
        req->fd = line = __real_open("/dev/null", O_RDWR);
    } else {
        assert(fd == line && op == GPIOHANDLE_SET_LINE_VALUES_IOCTL);
        level = va_arg(args, gpiohandle_data*)->values[0];
    }
    va_end(args);
    return 0;
}
ssize_t __wrap_write(int fd, const void* ptr, size_t size) {
    if (fd != bus) return __real_write(fd, ptr, size);
    if (interrupts > 0) { --interrupts; errno = EINTR; return -1; }
    auto data = static_cast<const uint8_t*>(ptr);
    assert((size == 1 || size == 2) && data[0] < 4);
    selected = data[0];
    if (size == 2) {
        if (short_writes) { --short_writes; return 1; }
        registers[selected] = data[1];
    }
    return size;
}
ssize_t __wrap_read(int fd, void* ptr, size_t size) {
    if (fd != bus) return __real_read(fd, ptr, size);
    assert(size == 1);
    *static_cast<uint8_t*>(ptr) = registers[selected];
    return 1;
}
}

int main() {
    char tmp[] = "/tmp/sa02m-io-XXXXXX";
    assert(mkdtemp(tmp));
    root = tmp;
    lockfile = root + "/lock";
    // .136 has an internal rtc0 and an unbound DS3231 I2C client. No rtc1 exists.
    namespace fs = std::filesystem;
    fs::path sys(root);
    fs::create_directories(sys / "class/rtc");
    fs::create_directories(sys / "devices/platform/soc/1c20400.rtc/rtc/rtc0");
    fs::create_directory_symlink(sys / "devices/platform/soc/1c20400.rtc/rtc/rtc0", sys / "class/rtc/rtc0");
    fs::path ds = sys / "bus/i2c/devices/1-0068", pcf = sys / "bus/i2c/devices/1-0051";
    fs::create_directories(ds);
    std::ofstream(ds / "name") << "ds3231,d1307\n";
    auto unavailable = [&] {
        try { readExternalRtc(sys); assert(false); } catch(const std::runtime_error&) {}
        assert(rtc_bus == -1);
    };
    auto external = readExternalRtc(sys);
    assert(external.tm_year == 126 && external.tm_mon == 10 && external.tm_mday == 23);
    assert(external.tm_hour == 10 && external.tm_min == 15 && external.tm_sec == 30 && rtc_reads == 2);
    rtc_status = 0x80; unavailable(); rtc_status = 0;
    rtc_data[5] = 2; rtc_data[4] = 0x29; unavailable();
    rtc_data[6] = 0x24; assert(readExternalRtc(sys).tm_mday == 29);
    rtc_data[2] = 0x72; assert(readExternalRtc(sys).tm_hour == 12);
    rtc_data[1] = 0x5a; unavailable(); rtc_data[1] = 0x15;
    rtc_bus_busy = true; unavailable(); rtc_bus_busy = false;
    fs::remove_all(ds);
    fs::create_directories(pcf);
    std::ofstream(pcf / "name") << "pcf8563\n";
    rtc_data = {0x30,0x15,0x10,0x23,0x01,0x11,0x26};
    assert(readExternalRtc(sys).tm_mday == 23);
    rtc_data[0] |= 0x80; unavailable();
    fs::remove_all(pcf); unavailable();
    assert(rtc0_opens == 0);  // The known Allwinner SoC node must never supply RTC_TIME.
    rtc_missing = false; rtc0_only = true;
    fs::remove(sys / "class/rtc/rtc0");
    fs::create_directories(sys / "devices/i2c-1/1-0068/rtc/rtc0");
    fs::create_directory_symlink(sys / "devices/i2c-1/1-0068/rtc/rtc0", sys / "class/rtc/rtc0");
    assert(readExternalRtc(sys).tm_year == 126 && rtc0_opens == 1); // External on /dev/rtc0 on an older image.
    rtc_missing = true; rtc0_only = false;
    puts("PASS: .136 internal rtc0 excluded, unbound DS3231 and PCF8563 burst reads, read-only register pointers, OSF/VL, invalid BCD/calendar, kernel ownership, external rtc0 compatibility");
    for (int bits = 0; bits < 16; ++bits)
        assert(build_output_mask(bits & 8, bits & 1, bits & 4, bits & 2) == (15 ^ bits));
    assert(applyOverride(15, "value=1\nexpires_at=101\n", 100) == 11);
    assert(applyOverride(2, "expires_at=101\nvalue=0\n", 100) == 6);
    for (auto invalid : {"value=1\nexpires_at=100\n", "value=2\nexpires_at=200\n",
                         "value=1\nexpires_at=101x\n", "value=1\nvalue=0\nexpires_at=200\n"})
        assert(applyOverride(15, invalid, 100) == 15);
    OutputState state;
    Pca9536Controller controller;
    const auto start = OutputState::Clock::now();
    auto sync = [&](uint8_t mask) { controller.writeOutputs(mask); };
    reject_address = true;
    try { state.update(13, start, sync); assert(false); } catch (const std::runtime_error&) {}
    assert(bus_closes == 1);
    reject_address = false;
    assert(!state.update(13, start + std::chrono::milliseconds(999), sync) && bus_opens == 1);
    short_writes = 1;
    try { state.update(13, start + std::chrono::seconds(1), sync); assert(false); } catch (const std::runtime_error&) {}
    interrupts = 2;
    assert(state.update(13, start + std::chrono::seconds(2), sync));
    assert(bus_opens == bus_closes && registers[1] == 13 && registers[3] == 0);
    registers[1] = 15;
    assert(state.update(13, start + std::chrono::seconds(3), sync) && registers[1] == 13);
    int holder = __real_open(lockfile.c_str(), O_RDWR);
    assert(holder >= 0 && flock(holder, LOCK_EX | LOCK_NB) == 0);
    try { sync(0); assert(false); } catch (const std::runtime_error&) {}
    assert(registers[1] == 13);
    __real_close(holder);
    { USBResetHelper usb; usb.set(false); assert(level == 0); usb.set(true); assert(level == 1); }
    assert(line == -1 && chip == -1);
    assert(uptimeDays(86400) == 1 && uptimeDays(43200) == 0.5f);
    ReusableSysInfo info;
    info.modem_path = root.c_str();
    std::ofstream(root + "/operstate") << "up\n";
    assert(get_modem_iface(info) == root && get_modem_status(info) == "up");
    info.modem_path = "/nonexistent/sa02m-test";
    assert(get_modem_status(info) == "NOT CONNECTED" && get_modem_iface(info) == "NOT CONNECTED");
    try { filesystemData(info.modem_path); assert(false); } catch (const std::runtime_error&) {}
    assert(get_ram_percent() >= 0 && filesystemData("/").size > 0);
    CA_02m io;
    auto addOutput = [&io](const char* name, bool value) {
        mplc::vm::Channel def{name};
        auto ch = io.Create(&def);
        ch->OutVar.Update(value);
        return ch;
    };
    addOutput("RED_LED", true); addOutput("BLUE_LED", false);
    addOutput("BUZZER", true); addOutput("DOUT", false); addOutput("USB_POWER", true);
    const int before = bus_opens;
    io.connected = false; io.Execute();
    assert(bus_opens == before && io.acks == 0);
    io.connected = true; reject_address = true; io.Execute();
    assert(io.fault && io.acks == 0 && level == 1);
    reject_address = false; io.Execute();
    assert(io.fault && bus_opens == before + 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1010));
    io.Execute();
    assert(!io.fault && io.acks == 1 && registers[1] == 10);
    CyntronProtocol diagnostic;
    mplc::vm::Channel rtcdef{"RTC_TIME"};
    auto rtc = diagnostic.Create(&rtcdef);
    diagnostic.Execute();
    assert(diagnostic.fault && rtc->InVar.quality == OpcUa_BadCommunicationError);
    rtc_missing = false;
    int reads_before = rtc_node_reads;
    diagnostic.Execute();
    assert(diagnostic.fault && rtc_node_reads == reads_before);
    std::this_thread::sleep_for(std::chrono::milliseconds(1010));
    diagnostic.Execute();
    assert(!diagnostic.fault && rtc->InVar.quality == 0);
    int reads_after = rtc_node_reads;
    diagnostic.Execute();
    assert(!diagnostic.fault && rtc_node_reads == reads_after);
    rtc->InVar.Get<mplc::DateTime>();
    auto check = [&diagnostic](const char* name, auto expectedType) {
        mplc::vm::Channel def{name};
        auto ch = diagnostic.Create(&def);
        diagnostic.Execute();
        if (ch->InVar.quality == OpcUa_BadCommunicationError) {
            assert(std::string(name) == "CPU_TEMP");  // Optional thermal node in the container.
        } else {
            ch->InVar.Get<decltype(expectedType)>();
        }
    };
    for (auto name : {"CPU_TEMP","CPU_LOAD","RAM_PERCENT","DISK_PERCENT","USB_FLASH_PERC","SD_CARD_PERC"})
        check(name, int16_t{});
    for (auto name : {"UPTIME","USB_FLASH_SIZE","USB_FLASH_USED","SD_CARD_SIZE","SD_CARD_USED"})
        check(name, float{});
    for (auto name : {"MODEM_IFACE","MODEM_STATUS"}) check(name, std::string{});
    for (auto name : {"USB_FLASH_DEV","SD_CARD_DEV"}) check(name, false);
    check("SYS_TIME", mplc::DateTime{});
    puts("PASS: 22 source channels, SDK test double, Fault and ACK on failed and recovered output, RTC quality recovery");
    std::filesystem::remove_all(root);
    puts("PASS: 16 output masks, TTL, I2C arbitration, EINTR, short write, fd cleanup, retry and readback, USB polarity, uptime, filesystem errors, modem channel order");
}
