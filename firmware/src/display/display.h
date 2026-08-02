#pragma once
#include <stdint.h>
#include <TFT_eSPI.h>

class Display {
public:
    Display();
    void begin();
    void splash(const char* text);

private:
    TFT_eSPI _tft;
    void _init_backlight();
};
