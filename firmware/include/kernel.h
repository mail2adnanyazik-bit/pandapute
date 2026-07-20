#pragma once
#include <stdint.h>

typedef enum {
    APP_DESKTOP = 0,
    APP_TERMINAL,
    APP_EDITOR,
    APP_FILES,
    APP_SETTINGS,
    APP_PIANO,
    APP_MIC,
    APP_TILT,
    APP_COUNT
} AppId;

typedef struct {
    const char *name;
    const char *icon;
    void (*on_create)();
    void (*on_destroy)();
    void (*on_focus)();
    void (*on_key)(uint8_t modifiers, uint8_t keycode);
    void (*on_tick)();
    void (*on_draw)();
} App;

void kernel_init();
void kernel_run_app(AppId id);
void kernel_set_status(const char *text);
const char *kernel_get_status();
AppId kernel_current_app();
void kernel_draw_status_bar();

extern App apps[APP_COUNT];
