#pragma once
#include <stdint.h>

enum AppId {
    APP_DESKTOP,
    APP_TERMINAL,
    APP_EDITOR,
    APP_FILES,
    APP_SETTINGS,
    APP_PIANO,
    APP_MIC,
    APP_TILT,
    APP_COUNT
};

void kernel_init();
void kernel_open(AppId id);
void kernel_key(uint8_t modifiers, uint8_t keycode);
void kernel_tick();
void kernel_set_status(const char* text);
void kernel_draw_status_bar();
AppId kernel_current();
const char* kernel_app_name(AppId id);
const char* kernel_app_icon(AppId id);
