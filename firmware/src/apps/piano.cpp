#include "apps.h"
#include "kernel.h"
#include "config.h"
#include "keyboard/keymap.h"
#include "audio/audio.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <math.h>

extern TFT_eSPI tft;
extern Audio g_audio;

#define NOTE_Y (STATUS_BAR_HEIGHT + 10)
#define NOTE_W 28
#define NOTE_H 100
#define START_X 10
#define NOTE_DURATION 600

static const uint16_t _freqs[8] = {
    262, 294, 330, 349, 392, 440, 494, 523
};
static const char* _notes[8] = {
    "C4", "D4", "E4", "F4", "G4", "A4", "B4", "C5"
};
static int _active_key = -1;
static unsigned long _note_start = 0;
static int _selected_octave = 4;

static void draw() {
    tft.fillRect(0, STATUS_BAR_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT - STATUS_BAR_HEIGHT, TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(10, NOTE_Y - 10);
    tft.printf("Octave: %d  Keys 1-8 = C4-C5", _selected_octave);

    for (int i = 0; i < 8; i++) {
        int x = START_X + i * (NOTE_W + 4);
        uint16_t color = (i == _active_key) ? TFT_YELLOW : TFT_WHITE;
        uint16_t fill = (i == _active_key) ? TFT_RED : TFT_DARKGREY;

        tft.fillRoundRect(x, NOTE_Y, NOTE_W, NOTE_H, 4, fill);
        tft.drawRoundRect(x, NOTE_Y, NOTE_W, NOTE_H, 4, color);

        tft.setTextColor(TFT_WHITE, fill);
        tft.setTextSize(1);
        int tw = tft.textWidth(_notes[i]);
        tft.setCursor(x + NOTE_W / 2 - tw / 2, NOTE_Y + NOTE_H / 2 - 4);
        tft.print(_notes[i]);
    }

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(10, DISPLAY_HEIGHT - 12);
    tft.print("1-8:Play  UP/DN:Octave  ESC:Exit");
}

static void _play_note(int idx) {
    if (idx < 0 || idx >= 8) return;

    _active_key = idx;
    _note_start = millis();

    int note_idx = idx + (_selected_octave - 4) * 12;
    if (note_idx < 0) note_idx = 0;
    uint16_t freq = 262 * powf(2.0f, note_idx / 12.0f);

    g_audio.play_tone(freq, NOTE_DURATION);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setCursor(10, DISPLAY_HEIGHT - 30);
    tft.printf("Playing: %s (%.0f Hz)", _notes[idx], freq);
}

void app_piano_init() {
    _active_key = -1;
    _selected_octave = 4;
    tft.fillScreen(TFT_BLACK);
    draw();
}

void app_piano_key(uint8_t mod, uint8_t key) {
    if (key == KEY_ESCAPE) {
        g_audio.stop();
        kernel_open(APP_DESKTOP);
        return;
    }

    if (key >= KEY_1 && key <= KEY_8) {
        _play_note(key - KEY_1);
    } else if (key == KEY_0) {
        _play_note(7);
    } else if (key == KEY_UP) {
        _selected_octave++;
        if (_selected_octave > 6) _selected_octave = 6;
        kernel_set_status("Octave +1");
    } else if (key == KEY_DOWN) {
        _selected_octave--;
        if (_selected_octave < 2) _selected_octave = 2;
        kernel_set_status("Octave -1");
    }

    draw();
}

void app_piano_tick() {
    if (_active_key >= 0 && millis() - _note_start > NOTE_DURATION) {
        _active_key = -1;
        draw();
    }
}
