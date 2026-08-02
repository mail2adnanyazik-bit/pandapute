#include "apps.h"
#include "kernel.h"
#include "config.h"
#include "keyboard/keymap.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

#define EDIT_LINES 12
#define EDIT_COLS 40
#define EDIT_X 4
#define EDIT_Y (STATUS_BAR_HEIGHT + 4)
#define EDIT_LINE_H 14

static char _lines[EDIT_LINES][EDIT_COLS + 1];
static int _cursor_row = 0;
static int _cursor_col = 0;
static bool _modified = false;
static unsigned long _cursor_blink = 0;
static bool _show_cursor = true;

static void draw() {
    tft.fillRect(0, STATUS_BAR_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT - STATUS_BAR_HEIGHT, TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);

    for (int i = 0; i < EDIT_LINES; i++) {
        tft.setCursor(EDIT_X, EDIT_Y + i * EDIT_LINE_H);
        tft.print(_lines[i]);
    }

    tft.setCursor(EDIT_X, EDIT_Y + _cursor_row * EDIT_LINE_H + EDIT_LINE_H + 4);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.printf("Ln %d Col %d  %s", _cursor_row + 1, _cursor_col + 1,
               _modified ? "[modified]" : "[saved]");
}

void app_editor_init() {
    memset(_lines, 0, sizeof(_lines));
    _cursor_row = 0;
    _cursor_col = 0;
    _modified = false;
    tft.fillScreen(TFT_BLACK);
    draw();
}

void app_editor_key(uint8_t mod, uint8_t key) {
    if (key == KEY_ESCAPE) {
        if (_modified) {
            kernel_set_status("Unsaved!");
            return;
        }
        kernel_open(APP_DESKTOP);
        return;
    }

    if (key == KEY_RETURN) {
        if (_cursor_row < EDIT_LINES - 1) {
            _cursor_row++;
            _cursor_col = 0;
            _modified = true;
        }
        draw();
        return;
    }

    if (key == KEY_BACKSPACE) {
        if (_cursor_col > 0) {
            int len = strlen(_lines[_cursor_row]);
            if (_cursor_col <= len) {
                for (int i = _cursor_col - 1; i < len; i++)
                    _lines[_cursor_row][i] = _lines[_cursor_row][i + 1];
            }
            _cursor_col--;
            _modified = true;
        } else if (_cursor_row > 0) {
            _cursor_col = strlen(_lines[_cursor_row - 1]);
            strcat(_lines[_cursor_row - 1], _lines[_cursor_row]);
            memset(_lines[_cursor_row], 0, EDIT_COLS + 1);
            for (int i = _cursor_row; i < EDIT_LINES - 1; i++)
                strcpy(_lines[i], _lines[i + 1]);
            _cursor_row--;
            _modified = true;
        }
        draw();
        return;
    }

    if (key == KEY_UP && _cursor_row > 0) _cursor_row--;
    else if (key == KEY_DOWN && _cursor_row < EDIT_LINES - 1) _cursor_row++;
    else if (key == KEY_LEFT && _cursor_col > 0) _cursor_col--;
    else if (key == KEY_RIGHT && _cursor_col < EDIT_COLS - 1) _cursor_col++;
    else {
        char c = 0;
        if (key >= KEY_A && key <= KEY_Z) {
            c = 'a' + (key - KEY_A);
            if (mod & 0x02) c -= 32;
        } else if (key == KEY_0) c = '0';
        else if (key >= KEY_1 && key <= KEY_9) c = '1' + (key - KEY_1);
        else if (key == KEY_SPACE) c = ' ';
        else if (key == KEY_TAB) c = '\t';
        else if (key == KEY_MINUS) c = '_';
        else if (key == KEY_EQUAL) c = '=';
        else if (key == KEY_SEMICOLON) c = ',';
        else if (key == KEY_QUOTE) c = '.';

        if (c) {
            int len = strlen(_lines[_cursor_row]);
            if (len < EDIT_COLS - 1) {
                for (int i = len; i >= _cursor_col; i--)
                    _lines[_cursor_row][i + 1] = _lines[_cursor_row][i];
                _lines[_cursor_row][_cursor_col] = c;
                _cursor_col++;
                if (_cursor_col >= EDIT_COLS) _cursor_col = EDIT_COLS - 1;
                _modified = true;
            }
        }
    }

    draw();
}

void app_editor_tick() {
    if (millis() - _cursor_blink > 400) {
        _cursor_blink = millis();
        _show_cursor = !_show_cursor;
        int x = EDIT_X + _cursor_col * 6;
        int y = EDIT_Y + _cursor_row * EDIT_LINE_H;
        tft.drawRect(x, y, 6, 12, _show_cursor ? TFT_YELLOW : TFT_BLACK);
    }
}
