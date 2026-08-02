#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "kernel.h"
#include "keyboard/keyboard.h"
#include "keyboard/keymap.h"
#include "audio/audio.h"
#include "power/power.h"
#include "touch/touch.h"
#include "sd_card/sd_card.h"
#include "usb_hid/usb_hid.h"
#include "mpu/mpu.h"

TFT_eSPI tft;
Keyboard g_keyboard;
Audio g_audio;
PowerManager g_power;
Touch g_touch;
SDCard g_sd;
HIDKeyboard g_hid;
MPU6050 g_mpu;

static unsigned long _last_battery_check = 0;
static uint8_t _hid_mod = 0;

static void _set_modifier(uint8_t keycode, bool on) {
    uint8_t bit = 0;
    if (keycode == KEY_LSHIFT || keycode == KEY_RSHIFT) bit = 0x02;
    else if (keycode == KEY_LCTRL || keycode == KEY_RCTRL) bit = 0x04;
    else if (keycode == KEY_LALT || keycode == KEY_RALT) bit = 0x08;

    if (bit) {
        if (on) _hid_mod |= bit;
        else _hid_mod &= ~bit;
    }
}

static void splash() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(3);
    int x = (tft.width() - tft.textWidth("Pandapute v1")) / 2;
    int y = (tft.height() - tft.fontHeight()) / 2;
    tft.setCursor(x, y);
    tft.print("Pandapute v1");
}

void setup() {
    Serial.begin(115200);
    delay(500);

    Serial.println("Pandapute v1");
    Serial.println("Booting...");

    SPI.begin(DISPLAY_SCLK_PIN, SPI_MISO_PIN, DISPLAY_MOSI_PIN, -1);

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    pinMode(DISPLAY_BL_PIN, OUTPUT);
    analogWrite(DISPLAY_BL_PIN, 200);
    splash();

    g_keyboard.begin();

    if (g_touch.begin()) Serial.println("Touch: ok");

    if (g_sd.begin())
        Serial.printf("SD: %llu MB\n", g_sd.total_bytes() / (1024 * 1024));

    if (g_audio.begin()) g_audio.play_tone(440, 150);

    g_power.begin();

    if (g_mpu.begin()) Serial.println("MPU: ok");

    g_hid.begin();

    kernel_init();
    kernel_set_status("Ready");
}

void loop() {
    unsigned long now = millis();

    g_keyboard.scan();
    for (int r = 0; r < KEYBOARD_ROWS; r++) {
        for (int c = 0; c < KEYBOARD_COLS; c++) {
            if (g_keyboard.key_just_pressed(r, c)) {
                uint8_t keycode = KEYMAP[r][c];

                _set_modifier(keycode, true);
                if (g_hid.is_connected()) g_hid.send_key(_hid_mod, keycode);
                kernel_key(_hid_mod, keycode);
            }

            if (g_keyboard.key_just_released(r, c)) {
                _set_modifier(KEYMAP[r][c], false);
                if (!g_keyboard.any_key_down()) g_hid.release_all();
            }
        }
    }

    TouchPoint tp = g_touch.read();
    if (tp.valid) {
        Serial.printf("Touch: %d,%d z=%d\n", tp.x, tp.y, tp.z);
    }

    if (now - _last_battery_check > 10000) {
        _last_battery_check = now;
        BatteryStatus bat = g_power.read_battery();
        char buf[24];
        snprintf(buf, sizeof(buf), "Bat: %d%%", bat.level_percent);
        kernel_set_status(buf);
    }

    kernel_tick();

    static unsigned long last_status = 0;
    if (now - last_status > 200) {
        last_status = now;
        kernel_draw_status_bar();
    }

    delay(KEYBOARD_POLL_MS);
}
