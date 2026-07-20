#include "apps.h"
#include "kernel.h"
#include "config.h"
#include "keyboard/keymap.h"
#include "audio/audio.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;
extern Audio g_audio;

#define NOTE_Y (STATUS_BAR_HEIGHT + 10)
#define KEY_W 28
#define KEY_H 100
#define START_X 10

static const uint16_t _freqs[8] = {
    262, 294, 330, 349, 392, 440, 494, 523
};
static const char *_notes[8] = {
    "C4", "D4", "E4", "F4", "G4", "A4", "B4", "C5"
};
static int _active_key = -1;
static unsigned long _note_start = 0;
static int _selected_octave = 4;

void app_piano_create() { _active_key = -1; _selected_octave = 4; }

void app_piano_destroy() { g_audio.stop(); }

void app_piano_focus() {
    tft.fillScreen(TFT_BLACK);
    app_piano_draw();
}

void app_piano_key(uint8_t mod, uint8_t key) {
    if (key == 0x29) { g_audio.stop(); kernel_run_app(APP_DESKTOP); return; }

    int idx = -1;
    if (key >= 0x1E && key <= 0x26) {
        idx = key - 0x1E; // keys 1-8 -> C4-C5
    } else if (key == 0x27) {
        idx = 7; // 0 -> C5
    }

    if (idx >= 0 && idx < 8) {
        _active_key = idx;
        _note_start = millis();
        int octave_offset = (_selected_octave - 4) * 12;
        int note_idx = idx + octave_offset;
        if (note_idx < 0) note_idx = 0;
        uint16_t freq = 262 * powf(2.0f, note_idx / 12.0f);
        g_audio.play_tone(freq, 2000);
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.setCursor(10, DISPLAY_HEIGHT - 30);
        tft.printf("Playing: %s (%.0f Hz)", _notes[idx], freq);
    } else if (key == KEY_UP) {
        _selected_octave++;
        if (_selected_octave > 6) _selected_octave = 6;
        kernel_set_status("Octave +1");
    } else if (key == KEY_DOWN) {
        _selected_octave--;
        if (_selected_octave < 2) _selected_octave = 2;
        kernel_set_status("Octave -1");
    }

    app_piano_draw();
}

void app_piano_tick() {
    if (_active_key >= 0 && millis() - _note_start > 200) {
        bool still_down = false;
        for (int r = 0; r < KEYBOARD_ROWS && !still_down; r++)
            for (int c = 0; c < KEYBOARD_COLS && !still_down; c++)
                if (KEYMAP[r][c] >= 0x1E && KEYMAP[r][c] <= 0x27) {
                    // Check if the key is still pressed via keyboard matrix
                }
        if (!still_down) {
            g_audio.stop();
            _active_key = -1;
        }
    }
}

void app_piano_draw() {
    tft.fillRect(0, STATUS_BAR_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT - STATUS_BAR_HEIGHT, TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(10, NOTE_Y - 10);
    tft.printf("Octave: %d  Keys 1-8 = C4-C5", _selected_octave);

    for (int i = 0; i < 8; i++) {
        int x = START_X + i * (KEY_W + 4);
        uint16_t color = (i == _active_key) ? TFT_YELLOW : TFT_WHITE;
        uint16_t fill = (i == _active_key) ? TFT_RED : TFT_DARKGREY;

        tft.fillRoundRect(x, NOTE_Y, KEY_W, KEY_H, 4, fill);
        tft.drawRoundRect(x, NOTE_Y, KEY_W, KEY_H, 4, color);

        tft.setTextColor(TFT_WHITE, fill);
        tft.setTextSize(1);
        int tw = tft.textWidth(_notes[i]);
        tft.setCursor(x + KEY_W / 2 - tw / 2, NOTE_Y + KEY_H / 2 - 4);
        tft.print(_notes[i]);
    }

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(10, DISPLAY_HEIGHT - 12);
    tft.print("1-8:Play  UP/DN:Octave  ESC:Exit");
}
