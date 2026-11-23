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
std::string root, lockfile;
}
extern "C" {
int __real_open(const char*, int, ...);
int __real_close(int);
ssize_t __real_read(int, void*, size_t);
ssize_t __real_write(int, const void*, size_t);
int __wrap_open(const char* path, int flags, ...) {
    if (!strcmp(path, "/dev/rtc0")) {
        if (rtc_missing) { errno = ENOENT; return -1; }
        return rtc_fd = __real_open("/dev/null", O_RDWR);
    }
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
    return __real_close(fd);
}
int __wrap_ioctl(int fd, unsigned long op, ...) {
    if (fd == bus) {
        if (op == I2C_SLAVE && reject_address) { errno = ENXIO; return -1; }
        return 0;
    }
    va_list args;
    va_start(args, op);
    if (fd == rtc_fd && op == RTC_RD_TIME) {
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
    diagnostic.Execute();
    assert(!diagnostic.fault && rtc->InVar.quality == 0);
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
