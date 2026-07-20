#include "apps.h"
#include "kernel.h"
#include "config.h"
#include "keyboard/keymap.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

#define TERM_LINES 15
#define TERM_COLS 40
#define TERM_X 4
#define TERM_Y (STATUS_BAR_HEIGHT + 4)
#define TERM_LINE_H 14

static char _buffer[TERM_LINES][TERM_COLS + 1];
static int _current_line = 0;
static int _scroll_offset = 0;
static char _input_line[TERM_COLS + 1];
static int _input_pos = 0;
static bool _show_cursor = true;
static unsigned long _cursor_blink = 0;

void app_terminal_create() {
    memset(_buffer, 0, sizeof(_buffer));
    memset(_input_line, 0, sizeof(_input_line));
    _current_line = 0;
    _input_pos = 0;
}

void app_terminal_destroy() {}

void app_terminal_focus() {
    tft.fillScreen(TFT_BLACK);
    _show_cursor = true;
    app_terminal_draw();
    strcpy(_buffer[_current_line], "> ");
    _input_pos = 2;
}

static void _terminal_putchar(char c) {
    if (_current_line >= TERM_LINES) {
        for (int i = 0; i < TERM_LINES - 1; i++)
            strcpy(_buffer[i], _buffer[i + 1]);
        _current_line = TERM_LINES - 1;
        memset(_buffer[_current_line], 0, TERM_COLS + 1);
    }
    int len = strlen(_buffer[_current_line]);
    if (len < TERM_COLS) {
        _buffer[_current_line][len] = c;
        _buffer[_current_line][len + 1] = '\0';
    }
}

static void _terminal_print(const char *s) {
    while (*s) {
        if (*s == '\n') {
            _current_line++;
        } else {
            _terminal_putchar(*s);
        }
        s++;
    }
}

void app_terminal_key(uint8_t mod, uint8_t key) {
    if (key == 0x29) {
        kernel_run_app(APP_DESKTOP);
        return;
    }

    if (key == KEY_RETURN) {
        _current_line++;
        if (_current_line >= TERM_LINES) {
            for (int i = 0; i < TERM_LINES - 1; i++)
                strcpy(_buffer[i], _buffer[i + 1]);
            _current_line = TERM_LINES - 1;
        }
        memset(_buffer[_current_line], 0, TERM_COLS + 1);

        if (strcmp(_input_line + 2, "help") == 0) {
            _terminal_print("\navailable: help, clear, echo, date, bat, apps, reboot\n");
        } else if (strcmp(_input_line + 2, "clear") == 0) {
            app_terminal_create();
        } else if (strncmp(_input_line + 2, "echo ", 5) == 0) {
            _terminal_print("\n");
            _terminal_print(_input_line + 7);
        } else if (strcmp(_input_line + 2, "bat") == 0) {
            _terminal_print("\nbattery: ?%");
        } else if (strcmp(_input_line + 2, "apps") == 0) {
            _terminal_print("\ndesktop, terminal, editor, files, settings");
        } else if (strcmp(_input_line + 2, "reboot") == 0) {
            ESP.restart();
        } else if (strlen(_input_line + 2) > 0) {
            _terminal_print("\nunknown: ");
            _terminal_print(_input_line + 2);
        }
        _current_line++;
        memset(_input_line, 0, sizeof(_input_line));
        strcpy(_input_line, "> ");
        _input_pos = 2;
        _show_cursor = true;
        app_terminal_draw();
        return;
    }

    if (key == KEY_BACKSPACE && _input_pos > 2) {
        _input_pos--;
        _input_line[_input_pos] = '\0';
        app_terminal_draw();
        return;
    }

    if (key >= 0x04 && key <= 0x1D) {
        char c = 'a' + (key - 0x04);
        if (mod & 0x02) c -= 32;
        if (_input_pos < TERM_COLS) {
            _input_line[_input_pos++] = c;
            _input_line[_input_pos] = '\0';
        }
    } else if (key == 0x27) {
        if (_input_pos < TERM_COLS) _input_line[_input_pos++] = '0';
    } else if (key >= 0x1E && key <= 0x26) {
        char c = '1' + (key - 0x1E);
        if (_input_pos < TERM_COLS) _input_line[_input_pos++] = c;
    } else if (key == 0x2C) {
        if (_input_pos < TERM_COLS) _input_line[_input_pos++] = ' ';
    } else if (key == 0x2B) {
        if (_input_pos < TERM_COLS) _input_line[_input_pos++] = '\t';
    } else if (key == KEY_UP) {
        if (_current_line > 0) {
            strcpy(_input_line, _buffer[_current_line - 1]);
            _input_pos = strlen(_input_line);
        }
    }

    app_terminal_draw();
}

void app_terminal_tick() {
    if (millis() - _cursor_blink > 500) {
        _cursor_blink = millis();
        _show_cursor = !_show_cursor;
        int y = TERM_Y + (_current_line + 1) * TERM_LINE_H;
        int x = TERM_X + _input_pos * 6;
        tft.drawRect(x, y, 6, 10, _show_cursor ? TFT_GREEN : TFT_BLACK);
    }
}

void app_terminal_draw() {
    tft.fillRect(0, STATUS_BAR_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT - STATUS_BAR_HEIGHT, TFT_BLACK);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(1);

    int start = _current_line - TERM_LINES + 1;
    if (start < 0) start = 0;

    for (int i = 0; i < TERM_LINES && (start + i) <= _current_line; i++) {
        tft.setCursor(TERM_X, TERM_Y + i * TERM_LINE_H);
        tft.print(_buffer[start + i]);
    }

    int cursor_y = TERM_Y + (_current_line - start + 1) * TERM_LINE_H;
    tft.setCursor(TERM_X, cursor_y);
    tft.print(_input_line);
    if (_show_cursor) {
        int cx = TERM_X + _input_pos * 6;
        tft.drawRect(cx, cursor_y, 6, 10, TFT_GREEN);
    }
}
