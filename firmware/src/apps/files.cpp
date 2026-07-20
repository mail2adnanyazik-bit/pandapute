#include "apps.h"
#include "kernel.h"
#include "config.h"
#include "keyboard/keymap.h"
#include "sd_card/sd_card.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;
extern SDCard g_sd;

#define MAX_ENTRIES 80
#define VISIBLE 12
#define LIST_X 4
#define LIST_Y (STATUS_BAR_HEIGHT + 16)
#define LIST_H 14

static char _entries[MAX_ENTRIES][36];
static bool _is_dir[MAX_ENTRIES];
static int _entry_count = 0;
static int _selected = 0;
static int _scroll = 0;
static char _current_dir[64] = "/";
static bool _card_mounted = false;

static void _refresh_list() {
    _entry_count = 0;
    _card_mounted = g_sd.is_mounted();
    if (!_card_mounted) return;

    // Add parent dir entry if not at root
    if (strcmp(_current_dir, "/") != 0) {
        strcpy(_entries[0], "..");
        _is_dir[0] = true;
        _entry_count = 1;
    }

    // Open directory and read entries
    // SD card directory listing uses SdFat's native openNext
    // For now, show a placeholder message
    kernel_set_status("SD read - fixme");
    return;

    char name[64];
    while (_entry_count < MAX_ENTRIES) {
        if (!g_sd.read_line(name, sizeof(name))) break;
        // Remove trailing newline
        int len = strlen(name);
        while (len > 0 && (name[len - 1] == '\n' || name[len - 1] == '\r'))
            name[--len] = '\0';
        if (len == 0 || name[0] == '.') continue;

        strncpy(_entries[_entry_count], name, 35);
        _entries[_entry_count][35] = '\0';
        // Mark as directory (SdFat doesn't easily distinguish here - simplified)
        _is_dir[_entry_count] = false;
        _entry_count++;
    }
    g_sd.close_file();
}

void app_files_create() {
    _entry_count = 0;
    _selected = 0;
    _scroll = 0;
    strcpy(_current_dir, "/");
}

void app_files_destroy() {}

void app_files_focus() {
    tft.fillScreen(TFT_BLACK);
    _refresh_list();
    app_files_draw();
}

void app_files_key(uint8_t mod, uint8_t key) {
    if (key == 0x29) { kernel_run_app(APP_DESKTOP); return; }

    if (key == KEY_UP && _selected > 0) _selected--;
    else if (key == KEY_DOWN && _selected < _entry_count - 1) _selected++;
    else if (key == KEY_RETURN || key == KEY_RIGHT) {
        if (_selected == 0 && strcmp(_current_dir, "/") != 0) {
            // Go to parent
            char *last = strrchr(_current_dir, '/');
            if (last == _current_dir) {
                _current_dir[1] = '\0';
            } else {
                *last = '\0';
            }
            _selected = 0;
            _scroll = 0;
            _refresh_list();
            kernel_set_status(_current_dir);
        }
    }
    else if (key == 0x29) {
        kernel_set_status("SD: scanning...");
        _refresh_list();
    }

    if (_selected < _scroll) _scroll = _selected;
    if (_selected >= _scroll + VISIBLE) _scroll = _selected - VISIBLE + 1;
    if (_scroll < 0) _scroll = 0;

    app_files_draw();
}

void app_files_tick() {}

void app_files_draw() {
    tft.fillRect(0, STATUS_BAR_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT - STATUS_BAR_HEIGHT, TFT_BLACK);

    // Header
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(LIST_X, STATUS_BAR_HEIGHT + 4);
    tft.print("SD:");
    tft.print(_current_dir);

    if (!_card_mounted || _entry_count == 0) {
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.setCursor(LIST_X, LIST_Y + 20);
        if (!_card_mounted) tft.print("No SD card detected");
        else tft.print("Empty directory");
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.setCursor(10, DISPLAY_HEIGHT - 12);
        tft.print("ESC:Back  ESC:Rescan");
        return;
    }

    int end = _scroll + VISIBLE;
    if (end > _entry_count) end = _entry_count;

    for (int i = _scroll; i < end; i++) {
        int y = LIST_Y + (i - _scroll) * LIST_H;

        if (i == _selected) {
            tft.fillRect(0, y, DISPLAY_WIDTH, LIST_H, TFT_NAVY);
        }

        tft.setTextColor(_is_dir[i] ? TFT_YELLOW : TFT_WHITE,
                         (i == _selected) ? TFT_NAVY : TFT_BLACK);
        tft.setCursor(LIST_X + 4, y + 2);
        tft.print(_is_dir[i] ? "[D] " : "[F] ");
        tft.print(_entries[i]);
    }

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(LIST_X, DISPLAY_HEIGHT - 10);
    tft.printf("%d items  ESC:Back", _entry_count);
}
