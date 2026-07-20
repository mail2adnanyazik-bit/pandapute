#include "apps.h"
#include "kernel.h"
#include "config.h"
#include "keyboard/keymap.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

#define ICON_SIZE 50
#define COLS 4
#define ROWS 2
#define START_Y (STATUS_BAR_HEIGHT + 20)

static const char *titles[APP_COUNT] = {
    "Desktop", "Terminal", "Editor", "Files", "Settings"
};
static int _selected = 0;
static int _scroll = 0;

void app_desktop_create() {
    _selected = 1;
    _scroll = 0;
}

void app_desktop_destroy() {}

void app_desktop_focus() {
    tft.fillScreen(TFT_BLACK);
    _selected = 1;
}

void app_desktop_key(uint8_t mod, uint8_t key) {
    int cols = COLS;
    int item = 0;
    switch (key) {
        case KEY_LEFT:
            _selected--;
            if (_selected < 1) _selected = APP_COUNT - 1;
            break;
        case KEY_RIGHT:
            _selected++;
            if (_selected >= APP_COUNT) _selected = 1;
            break;
        case KEY_UP:
            _selected -= cols;
            if (_selected < 1) _selected = 1;
            break;
        case KEY_DOWN:
            _selected += cols;
            if (_selected >= APP_COUNT) _selected = APP_COUNT - 1;
            break;
        case KEY_RETURN:
        case ' ':
            if (_selected > 0 && _selected < APP_COUNT) {
                kernel_run_app((AppId)_selected);
            }
            break;
        case 0x29:
            kernel_set_status("Desktop");
            break;
    }
    app_desktop_draw();
}

void app_desktop_tick() {}

void app_desktop_draw() {
    tft.fillRect(0, STATUS_BAR_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT - STATUS_BAR_HEIGHT, TFT_BLACK);

    int idx = 1;
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (idx >= APP_COUNT) break;
            int x = 10 + c * (ICON_SIZE + 20);
            int y = START_Y + r * (ICON_SIZE + 30);

            if (idx == _selected) {
                tft.fillRoundRect(x - 4, y - 4, ICON_SIZE + 8, ICON_SIZE + 8, 6, TFT_NAVY);
            }

            tft.fillRoundRect(x, y, ICON_SIZE, ICON_SIZE, 4, TFT_DARKGREY);
            tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
            tft.setTextSize(2);
            tft.setCursor(x + ICON_SIZE / 2 - 6, y + ICON_SIZE / 2 - 8);
            tft.print(apps[idx].icon);

            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.setTextSize(1);
            int tw = tft.textWidth(titles[idx]);
            tft.setCursor(x + ICON_SIZE / 2 - tw / 2, y + ICON_SIZE + 6);
            tft.print(titles[idx]);

            idx++;
        }
    }

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(4, DISPLAY_HEIGHT - 12);
    tft.print("ESC:Term  Enter:Open");
}
