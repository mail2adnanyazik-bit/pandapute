#include "apps.h"
#include "kernel.h"
#include "config.h"
#include "keyboard/keymap.h"
#include "audio/audio.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;
extern Audio g_audio;

#define ITEMS_Y (STATUS_BAR_HEIGHT + 10)
#define ITEM_H 30

typedef struct {
    const char *label;
    int  value;
    int  min;
    int  max;
    int  step;
    char unit[8];
    void (*on_change)(int val);
} Setting;

static void _set_brightness(int val) {
    analogWrite(DISPLAY_BL_PIN, val);
}

static void _set_volume(int val) {
    g_audio.set_volume(val);
    g_audio.play_tone(440, 50);
}

static Setting _settings[] = {
    { "Brightness", 200, 0,   255, 5,  "",   _set_brightness },
    { "Volume",      50, 0,   100, 5,  "%",  _set_volume     },
    { "USB HID",      1, 0,     1, 1,  "",   nullptr         },
    { "Touch Cal",    0, 0,     1, 1,  "",   nullptr         },
    { "Debug Ser",    1, 0,     1, 1,  "",   nullptr         },
};
static const int _setting_count = sizeof(_settings) / sizeof(_settings[0]);
static int _selected = 0;

void app_settings_create() { _selected = 0; }
void app_settings_destroy() {}

void app_settings_focus() {
    tft.fillScreen(TFT_BLACK);
    app_settings_draw();
}

void app_settings_key(uint8_t mod, uint8_t key) {
    if (key == 0x29) { kernel_run_app(APP_DESKTOP); return; }

    if (key == KEY_UP && _selected > 0) _selected--;
    else if (key == KEY_DOWN && _selected < _setting_count - 1) _selected++;
    else if (key == KEY_LEFT) {
        Setting *s = &_settings[_selected];
        s->value -= s->step;
        if (s->value < s->min) s->value = s->min;
        if (s->on_change) s->on_change(s->value);
        kernel_set_status(s->label);
    }
    else if (key == KEY_RIGHT) {
        Setting *s = &_settings[_selected];
        s->value += s->step;
        if (s->value > s->max) s->value = s->max;
        if (s->on_change) s->on_change(s->value);
        kernel_set_status(s->label);
    }

    app_settings_draw();
}

void app_settings_tick() {}

void app_settings_draw() {
    tft.fillRect(0, STATUS_BAR_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT - STATUS_BAR_HEIGHT, TFT_BLACK);
    tft.setTextSize(1);

    for (int i = 0; i < _setting_count; i++) {
        int y = ITEMS_Y + i * ITEM_H;

        if (i == _selected) {
            tft.fillRect(0, y, DISPLAY_WIDTH, ITEM_H, TFT_NAVY);
        }

        tft.setTextColor(TFT_WHITE, (i == _selected) ? TFT_NAVY : TFT_BLACK);
        tft.setCursor(10, y + 4);
        tft.print(_settings[i].label);

        tft.setCursor(10, y + 16);

        if (_settings[i].max == 1) {
            tft.print(_settings[i].value ? "ON " : "OFF");
            tft.setTextColor(TFT_GREEN, (i == _selected) ? TFT_NAVY : TFT_BLACK);
            tft.setCursor(60, y + 16);
            tft.print(" < >");
        } else {
            tft.print("[");
            int bar_w = map(_settings[i].value, _settings[i].min, _settings[i].max, 0, 100);
            for (int b = 0; b < 100; b += 5) {
                tft.fillRect(70 + b * 2, y + 16, 8, 8, (b < bar_w) ? TFT_GREEN : TFT_DARKGREY);
            }
            tft.print("] ");
            tft.print(_settings[i].value);
            tft.print(_settings[i].unit);
        }
    }

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setCursor(4, DISPLAY_HEIGHT - 10);
    tft.print("ESC:Back  < >:Adjust");
}
