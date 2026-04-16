#include "test_fb.h"

void TestFB::Execute() {

    uint8_t mask;
    mask = 0xFF;

    if (Blue_Led)  {mask = mask & ~((uint8_t)1 << 3);}
    if (Red_Led)   {mask = mask & ~((uint8_t)1 << 0);}
    if (Buzzer)    {mask = mask & ~((uint8_t)1 << 2);}
    if (Dout) {mask = mask & ~((uint8_t)1 << 1);}

    if (!Init_CM) {
	system("i2cset -y 2 0x41 0x03 0x00");
    Init_CM = TRUE;
    }
    // по переднему фронту
    if (!was_USB_Power && USB_Power) { USB_Reset = TRUE; USB_Counter = 0;}

    if (USB_Reset)
    {
	USB_Counter++;
	if (USB_Counter > 100) {USB_Reset = FALSE; USB_Counter = 0;}
    }

    if (USB_Reset)
    {
	system("gpioset 0 268=0");
    }
    else
    {
	system("gpioset 0 268=1");
    }

    std::string cmd = "i2cset -y 2 0x41 0x01 " + std::to_string(mask);
    system(cmd.c_str());

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
