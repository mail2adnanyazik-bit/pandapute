#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "kernel.h"
#include "keyboard/keyboard.h"
#include "keyboard/keymap.h"
#include "display/display.h"
#include "audio/audio.h"
#include "power/power.h"
#include "touch/touch.h"
#include "sd_card/sd_card.h"
#include "usb_hid/usb_hid.h"
#include "mpu/mpu.h"

// Globals
TFT_eSPI     tft;
Keyboard     g_keyboard;
Display      g_display;
Audio        g_audio;
PowerManager g_power;
Touch        g_touch;
SDCard       g_sd;
HIDKeyboard  g_hid;
MPU6050      g_mpu;

static unsigned long g_last_battery_check = 0;

void setup() {
    Serial.begin(115200);
    delay(500);

    Serial.println("\n\nPandapute v1.0");
    Serial.println("Booting...");

    SPI.begin(DISPLAY_SCLK_PIN, SPI_MISO_PIN, DISPLAY_MOSI_PIN, -1);

    g_display.begin();
    g_display.splash("Pandapute v1");

    g_keyboard.begin(); Serial.println("KB: OK");

    if (g_touch.begin())  Serial.println("Touch: OK");
    else                  Serial.println("Touch: --");

    if (g_sd.begin())     Serial.printf("SD: %llu MB\n", g_sd.total_bytes() / (1024 * 1024));
    else                  Serial.println("SD: --");

    if (g_audio.begin()) { Serial.println("Audio: OK"); g_audio.play_tone(440, 150); }
    else                  Serial.println("Audio: --");

    if (g_power.begin())  Serial.println("Power: OK");
    else                  Serial.println("Power: --");

    if (g_mpu.begin())    Serial.println("MPU: OK");
    else                  Serial.println("MPU: --");

    g_hid.begin();
    Serial.println("HID: OK");

    kernel_init();
    kernel_set_status("Ready");
    Serial.println("Pandapute ready.");
}

static uint8_t _hid_mod = 0;

void loop() {
    unsigned long now = millis();

    // --- Keyboard ---
    g_keyboard.scan();
    for (int r = 0; r < KEYBOARD_ROWS; r++) {
        for (int c = 0; c < KEYBOARD_COLS; c++) {
            if (g_keyboard.key_just_pressed(r, c)) {
                uint8_t keycode = KEYMAP[r][c];
                Serial.printf("Key: %02X\n", keycode);

                switch (keycode) {
                    case KEY_LSHIFT: _hid_mod |= 0x02; break;
                    case KEY_LCTRL:  _hid_mod |= 0x04; break;
                    case KEY_LALT:   _hid_mod |= 0x08; break;
                    default: break;
                }

                if (g_hid.is_connected()) {
                    g_hid.send_key(_hid_mod, keycode);
                }

                AppId app = kernel_current_app();
                if (apps[app].on_key) {
                    apps[app].on_key(_hid_mod, keycode);
                }
            }

            if (g_keyboard.key_just_released(r, c)) {
                uint8_t keycode = KEYMAP[r][c];
                switch (keycode) {
                    case KEY_LSHIFT: _hid_mod &= ~0x02; break;
                    case KEY_LCTRL:  _hid_mod &= ~0x04; break;
                    case KEY_LALT:   _hid_mod &= ~0x08; break;
                    default: break;
                }
                if (!g_keyboard.any_key_down()) {
                    g_hid.release_all();
                }
            }
        }
    }

    // --- Touch ---
    TouchPoint tp = g_touch.read();
    if (tp.valid) {
        Serial.printf("Touch: %d,%d z=%d\n", tp.x, tp.y, tp.z);
    }

    // --- Battery (every 10s) ---
    if (now - g_last_battery_check > 10000) {
        g_last_battery_check = now;
        BatteryStatus bat = g_power.read_battery();
        Serial.printf("Bat: %d%% chg=%d done=%d\n",
                      bat.level_percent, bat.charging, bat.charged);
        char buf[24];
        snprintf(buf, sizeof(buf), "Bat: %d%%", bat.level_percent);
        kernel_set_status(buf);
    }

    // --- App tick ---
    AppId app = kernel_current_app();
    if (apps[app].on_tick) apps[app].on_tick();

    // --- Status bar (200ms) ---
    static unsigned long last_status = 0;
    if (now - last_status > 200) {
        last_status = now;
        kernel_draw_status_bar();
    }

    delay(KEYBOARD_POLL_MS);
}
