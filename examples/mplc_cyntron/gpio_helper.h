#pragma once
#include <memory>
namespace mplc { namespace cyntron {

class USBResetHelper {
public:
    USBResetHelper();
    void On();
    void Off();
    struct internal;
    struct internal_deleter {
        void operator()(internal*);
    };
    private:
    bool on = true;
    std::unique_ptr<internal, internal_deleter> gpio_data;
};
}}  // namespace mplc::cyntron
