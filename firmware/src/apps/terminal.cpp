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
static bool _show_cursor = true;
static unsigned long _cursor_blink = 0;

static void _putchar(char c) {
    if (_current_line >= TERM_LINES) {
        for (int i = 0; i < TERM_LINES - 1; i++)
            strcpy(_buffer[i], _buffer[i + 1]);
        _current_line = TERM_LINES - 1;
    }
    int len = strlen(_buffer[_current_line]);
    if (len < TERM_COLS - 1) {
        _buffer[_current_line][len] = c;
        _buffer[_current_line][len + 1] = '\0';
    }
}

static void _print(const char* s) {
    while (*s) {
        if (*s == '\n') {
            _current_line++;
            if (_current_line >= TERM_LINES) {
                for (int i = 0; i < TERM_LINES - 1; i++)
                    strcpy(_buffer[i], _buffer[i + 1]);
                _current_line = TERM_LINES - 1;
            }
        } else {
            _putchar(*s);
        }
        s++;
    }
}

static void _run_command(const char* cmd) {
    if (*cmd == '\0') return;

    if (strcmp(cmd, "help") == 0) {
        _print("\navailable: help, clear, echo, bat, apps, reboot\n");
    } else if (strcmp(cmd, "clear") == 0) {
        memset(_buffer, 0, sizeof(_buffer));
        _current_line = 0;
    } else if (strncmp(cmd, "echo ", 5) == 0) {
        _print("\n");
        _print(cmd + 5);
    } else if (strcmp(cmd, "bat") == 0) {
        _print("\nbattery: ?%");
    } else if (strcmp(cmd, "apps") == 0) {
        _print("\ndesktop, terminal, editor, files, settings");
    } else if (strcmp(cmd, "reboot") == 0) {
        ESP.restart();
    } else {
        _print("\nunknown: ");
        _print(cmd);
    }
}

static int _scroll_start() {
    int start = _current_line - TERM_LINES + 1;
    return start < 0 ? 0 : start;
}

static void draw() {
    tft.fillRect(0, STATUS_BAR_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT - STATUS_BAR_HEIGHT, TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(1);

    int start = _scroll_start();

    for (int i = start; i <= _current_line; i++) {
        tft.setCursor(TERM_X, TERM_Y + (i - start) * TERM_LINE_H);
        tft.print(_buffer[i]);
    }

    if (_show_cursor) {
        int cx = TERM_X + strlen(_buffer[_current_line]) * 6;
        int cy = TERM_Y + (_current_line - start) * TERM_LINE_H;
        tft.drawRect(cx, cy, 6, 10, TFT_GREEN);
    }
}

void app_terminal_init() {
    memset(_buffer, 0, sizeof(_buffer));
    _current_line = 0;
    strcpy(_buffer[0], "> ");
    _show_cursor = true;
    tft.fillScreen(TFT_BLACK);
    draw();
}

void app_terminal_key(uint8_t mod, uint8_t key) {
    if (key == KEY_ESCAPE) {
        kernel_open(APP_DESKTOP);
        return;
    }

    if (key == KEY_RETURN) {
        _run_command(_buffer[_current_line] + 2);
        _current_line++;
        if (_current_line >= TERM_LINES) {
            for (int i = 0; i < TERM_LINES - 1; i++)
                strcpy(_buffer[i], _buffer[i + 1]);
            _current_line = TERM_LINES - 1;
        }
        strcpy(_buffer[_current_line], "> ");
        _show_cursor = true;
        draw();
        return;
    }

    if (key == KEY_BACKSPACE) {
        int len = strlen(_buffer[_current_line]);
        if (len > 2) _buffer[_current_line][len - 1] = '\0';
        draw();
        return;
    }

    if (key >= KEY_A && key <= KEY_Z) {
        char c = 'a' + (key - KEY_A);
        if (mod & 0x02) c -= 32;
        _putchar(c);
    } else if (key == KEY_0) {
        _putchar('0');
    } else if (key >= KEY_1 && key <= KEY_9) {
        _putchar('1' + (key - KEY_1));
    } else if (key == KEY_SPACE) {
        _putchar(' ');
    } else if (key == KEY_TAB) {
        _putchar('\t');
    } else if (key == KEY_UP) {
        if (_current_line > 0) {
            strcpy(_buffer[_current_line], _buffer[_current_line - 1]);
        }
    }

    draw();
}

void app_terminal_tick() {
    if (millis() - _cursor_blink > 500) {
        _cursor_blink = millis();
        _show_cursor = !_show_cursor;
        int cx = TERM_X + strlen(_buffer[_current_line]) * 6;
        int cy = TERM_Y + (_current_line - _scroll_start()) * TERM_LINE_H;
        tft.drawRect(cx, cy, 6, 10, _show_cursor ? TFT_GREEN : TFT_BLACK);
    }
}
