#include "apps.h"
#include "kernel.h"
#include "config.h"
#include "keyboard/keymap.h"
#include "audio/audio.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;
extern Audio g_audio;

#define BAR_X 20
#define BAR_Y (STATUS_BAR_HEIGHT + 40)
#define BAR_W (DISPLAY_WIDTH - 40)
#define BAR_H 30

static float _level = 0;
static bool _recording = false;
static unsigned long _last_poll = 0;

void app_mic_create() { _level = 0; _recording = false; }

void app_mic_destroy() {}

void app_mic_focus() {
    tft.fillScreen(TFT_BLACK);
    app_mic_draw();
}

void app_mic_key(uint8_t mod, uint8_t key) {
    if (key == 0x29) { kernel_run_app(APP_DESKTOP); return; }
    if (key == KEY_SPACE || key == KEY_RETURN) {
        _recording = !_recording;
        kernel_set_status(_recording ? "Listening..." : "Paused");
    }
    app_mic_draw();
}

void app_mic_tick() {
    if (!_recording) return;

    unsigned long now = millis();
    if (now - _last_poll < 50) return;
    _last_poll = now;

    float new_level = g_audio.get_mic_level();

    // Smooth
    _level = _level * 0.7f + new_level * 0.3f;
    if (_level > 1.0f) _level = 1.0f;

    app_mic_draw();
}

void app_mic_draw() {
    tft.fillRect(0, STATUS_BAR_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT - STATUS_BAR_HEIGHT, TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, STATUS_BAR_HEIGHT + 10);
    tft.print("Microphone");

    // Level bar background
    tft.drawRect(BAR_X, BAR_Y, BAR_W, BAR_H, TFT_DARKGREY);

    // Level bar fill
    int fill_w = (int)(_level * BAR_W);
    if (fill_w > BAR_W) fill_w = BAR_W;
    uint16_t bar_color;
    if (_level < 0.3f) bar_color = TFT_GREEN;
    else if (_level < 0.7f) bar_color = TFT_YELLOW;
    else bar_color = TFT_RED;

    tft.fillRect(BAR_X + 1, BAR_Y + 1, fill_w, BAR_H - 2, bar_color);

    // Percentage
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(BAR_X + BAR_W / 2 - 20, BAR_Y + BAR_H + 8);
    tft.printf("%d%%", (int)(_level * 100));

    // Status
    tft.setTextSize(1);
    tft.setCursor(BAR_X, BAR_Y + BAR_H + 30);
    tft.setTextColor(_recording ? TFT_GREEN : TFT_DARKGREY, TFT_BLACK);
    tft.printf("Status: %s", _recording ? "RECORDING" : "PAUSED");

    // Controls
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setCursor(10, DISPLAY_HEIGHT - 12);
    tft.print("SPACE:Record  ESC:Exit");
}
