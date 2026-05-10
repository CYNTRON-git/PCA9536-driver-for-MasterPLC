#include "test_fb.h"

void TestFB::Execute() {
    const uint8_t mask = cyntron_ca02m::build_output_mask(Blue_Led, Red_Led, Buzzer, Dout);
    // по переднему фронту
    if (!was_USB_Power && USB_Power) { USB_Reset = TRUE; USB_Counter = 0;}

    if (USB_Reset)
    {
	USB_Counter++;
	if (USB_Counter > 100) {USB_Reset = FALSE; USB_Counter = 0;}
    }

    try {
        if (!outputs_initialized || mask != last_mask) {
            pca9536_.writeOutputs(mask);
            last_mask = mask;
            outputs_initialized = true;
        }
        usb_power_.setEnabled(!USB_Reset);
        SetEnO(true);
    } catch (const std::exception&) {
        outputs_initialized = false;
        SetEnO(false);
    }

    was_USB_Power = USB_Power;
    
}

MPLC_DECLARE_PROPERTIES(TestFB) {
        MPLC_In(Blue_Led),
        MPLC_In(Red_Led),
        MPLC_In(Buzzer),
        MPLC_In(USB_Power),
        MPLC_In(Dout),
};

MPLC_FB_TYPE(CA_02m, TestFB);
