#include "display.h"
#include "config.h"
#include <Arduino.h>

Display::Display() : _tft() {}

void Display::begin() {
    _tft.init();
    _tft.setRotation(1);
    _tft.fillScreen(TFT_BLACK);
    _init_backlight();
}

void Display::_init_backlight() {
    pinMode(DISPLAY_BL_PIN, OUTPUT);
    analogWrite(DISPLAY_BL_PIN, 200);
}

void Display::splash(const char* text) {
    _tft.fillScreen(TFT_BLACK);
    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setTextSize(3);
    int x = (_tft.width() - _tft.textWidth(text)) / 2;
    int y = (_tft.height() - _tft.fontHeight()) / 2;
    _tft.setCursor(x, y);
    _tft.println(text);

    _tft.setTextSize(1);
    _tft.setCursor(_tft.width() / 2 - 50, y + 30);
    _tft.println("Booting...");
}

void Display::clear(uint16_t color) {
    _tft.fillScreen(color);
}

void Display::set_backlight(uint8_t brightness) {
    analogWrite(DISPLAY_BL_PIN, brightness);
}

TFT_eSPI& Display::tft() { return _tft; }
