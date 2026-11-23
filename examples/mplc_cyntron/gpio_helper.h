#pragma once

namespace mplc::cyntron {
class USBResetHelper {
public:
    USBResetHelper() = default;
    ~USBResetHelper();
    USBResetHelper(const USBResetHelper&) = delete;
    USBResetHelper& operator=(const USBResetHelper&) = delete;
    void set(bool enabled);
private:
    int line_fd = -1;
    bool level = false;
};
}
