#pragma once
#include <mplc/api.h>

class TestFB : public mplc::api::ScadaFB {
    BOOL Blue_Led;
    BOOL Red_Led;
    BOOL Buzzer;
    BOOL USB_Power;
    BOOL Dout;

    BOOL was_USB_Power;
    BOOL USB_Reset;
    INT  USB_Counter;

    BOOL Init_CM = FALSE;
public:
    MPLC_OBJECT(TestFB);
    void Execute() override;
};

