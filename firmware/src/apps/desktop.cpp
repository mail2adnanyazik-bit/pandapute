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

static int _selected = 1;

static void draw() {
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
            tft.print(kernel_app_icon((AppId)idx));

            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.setTextSize(1);
            int tw = tft.textWidth(kernel_app_name((AppId)idx));
            tft.setCursor(x + ICON_SIZE / 2 - tw / 2, y + ICON_SIZE + 6);
            tft.print(kernel_app_name((AppId)idx));

            idx++;
        }
    }

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(4, DISPLAY_HEIGHT - 12);
    tft.print("Enter:Open");
}

void app_desktop_init() {
    _selected = 1;
    tft.fillScreen(TFT_BLACK);
    draw();
}

void app_desktop_key(uint8_t mod, uint8_t key) {
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
            _selected -= COLS;
            if (_selected < 1) _selected = 1;
            break;
        case KEY_DOWN:
            _selected += COLS;
            if (_selected >= APP_COUNT) _selected = APP_COUNT - 1;
            break;
        case KEY_RETURN:
        case KEY_SPACE:
            kernel_open((AppId)_selected);
            break;
    }
    draw();
}
