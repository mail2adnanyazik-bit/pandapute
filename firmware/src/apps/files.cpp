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
#define ROW_H 14

static char _entries[MAX_ENTRIES][SD_MAX_NAME];
static bool _is_dir[MAX_ENTRIES];
static int _entry_count = 0;
static int _selected = 0;
static int _scroll = 0;
static char _current_dir[64] = "/";

static void _refresh_list() {
    _entry_count = 0;
    if (!g_sd.is_mounted()) return;

    if (strcmp(_current_dir, "/") != 0) {
        strcpy(_entries[0], "..");
        _is_dir[0] = true;
        _entry_count = 1;
    }

    _entry_count += g_sd.list_dir(_current_dir, &_entries[_entry_count],
                                  &_is_dir[_entry_count], MAX_ENTRIES - _entry_count);
}

static void _open_selected() {
    if (_selected == 0 && strcmp(_current_dir, "/") != 0) {
        char* last = strrchr(_current_dir, '/');
        if (last == _current_dir) _current_dir[1] = '\0';
        else *last = '\0';
        _selected = 0;
        _scroll = 0;
        _refresh_list();
        kernel_set_status(_current_dir);
        return;
    }

    if (_selected < _entry_count && _is_dir[_selected]) {
        char path[64];
        if (strcmp(_current_dir, "/") == 0)
            snprintf(path, sizeof(path), "/%s", _entries[_selected]);
        else
            snprintf(path, sizeof(path), "%s/%s", _current_dir, _entries[_selected]);
        strcpy(_current_dir, path);
        _selected = 0;
        _scroll = 0;
        _refresh_list();
        kernel_set_status(_current_dir);
    }
}

static void draw() {
    tft.fillRect(0, STATUS_BAR_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT - STATUS_BAR_HEIGHT, TFT_BLACK);

    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(LIST_X, STATUS_BAR_HEIGHT + 4);
    tft.print("SD:");
    tft.print(_current_dir);

    if (!g_sd.is_mounted() || _entry_count == 0) {
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.setCursor(LIST_X, LIST_Y + 20);
        if (!g_sd.is_mounted()) tft.print("No SD card detected");
        else tft.print("Empty directory");
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.setCursor(10, DISPLAY_HEIGHT - 12);
        tft.print("ESC:Exit");
        return;
    }

    int end = _scroll + VISIBLE;
    if (end > _entry_count) end = _entry_count;

    for (int i = _scroll; i < end; i++) {
        int y = LIST_Y + (i - _scroll) * ROW_H;

        if (i == _selected) {
            tft.fillRect(0, y, DISPLAY_WIDTH, ROW_H, TFT_NAVY);
        }

        tft.setTextColor(_is_dir[i] ? TFT_YELLOW : TFT_WHITE,
                         (i == _selected) ? TFT_NAVY : TFT_BLACK);
        tft.setCursor(LIST_X + 4, y + 2);
        tft.print(_is_dir[i] ? "[D] " : "[F] ");
        tft.print(_entries[i]);
    }

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setCursor(LIST_X, DISPLAY_HEIGHT - 10);
    tft.printf("%d items  Enter:Open  ESC:Exit", _entry_count);
}

void app_files_init() {
    strcpy(_current_dir, "/");
    _selected = 0;
    _scroll = 0;
    _refresh_list();
    tft.fillScreen(TFT_BLACK);
    draw();
}

void app_files_key(uint8_t mod, uint8_t key) {
    if (key == KEY_ESCAPE) {
        kernel_open(APP_DESKTOP);
        return;
    }

    if (key == KEY_UP && _selected > 0) _selected--;
    else if (key == KEY_DOWN && _selected < _entry_count - 1) _selected++;
    else if (key == KEY_RETURN || key == KEY_RIGHT) _open_selected();

    if (_selected < _scroll) _scroll = _selected;
    if (_selected >= _scroll + VISIBLE) _scroll = _selected - VISIBLE + 1;
    if (_scroll < 0) _scroll = 0;

    draw();
}
