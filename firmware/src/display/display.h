#pragma once
#include <stdint.h>
#include <TFT_eSPI.h>

class Display {
public:
    Display();
    void begin();
    void splash(const char* text);
    void clear(uint16_t color);
    void set_backlight(uint8_t brightness);

    TFT_eSPI& tft();

private:
    TFT_eSPI _tft;
    void _init_backlight();
};
