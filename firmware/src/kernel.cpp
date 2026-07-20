#include "kernel.h"
#include "apps.h"
#include "config.h"
#include "power/power.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;
extern PowerManager g_power;

static AppId _current_app = APP_DESKTOP;
static char _status_text[32] = "Ready";
static unsigned long _last_status_clear = 0;
static unsigned long _last_bat_check = 0;
static int _bat_pct = -1;

App apps[APP_COUNT] = {
    { "Desktop",     "~", app_desktop_create,  app_desktop_destroy,  app_desktop_focus,  app_desktop_key,  app_desktop_tick,  app_desktop_draw  },
    { "Terminal",    ">", app_terminal_create, app_terminal_destroy, app_terminal_focus, app_terminal_key, app_terminal_tick, app_terminal_draw },
    { "Editor",      "E", app_editor_create,   app_editor_destroy,   app_editor_focus,   app_editor_key,   app_editor_tick,   app_editor_draw   },
    { "Files",       "F", app_files_create,    app_files_destroy,    app_files_focus,    app_files_key,    app_files_tick,    app_files_draw    },
    { "Settings",    "S", app_settings_create, app_settings_destroy, app_settings_focus, app_settings_key, app_settings_tick, app_settings_draw },
    { "Piano",       "P", app_piano_create,    app_piano_destroy,    app_piano_focus,    app_piano_key,    app_piano_tick,    app_piano_draw    },
    { "Mic",         "M", app_mic_create,      app_mic_destroy,      app_mic_focus,      app_mic_key,      app_mic_tick,      app_mic_draw      },
    { "Tilt",        "T", app_tilt_create,     app_tilt_destroy,     app_tilt_focus,     app_tilt_key,     app_tilt_tick,     app_tilt_draw     },
};

void kernel_init() {
    for (int i = 0; i < APP_COUNT; i++) {
        if (apps[i].on_create) apps[i].on_create();
    }
    _current_app = APP_DESKTOP;
    if (apps[_current_app].on_focus) apps[_current_app].on_focus();
}

void kernel_run_app(AppId id) {
    if (id < 0 || id >= APP_COUNT || id == _current_app) return;
    if (apps[_current_app].on_destroy) apps[_current_app].on_destroy();
    _current_app = id;
    if (apps[_current_app].on_create) apps[_current_app].on_create();
    if (apps[_current_app].on_focus) apps[_current_app].on_focus();
}

void kernel_set_status(const char *text) {
    strncpy(_status_text, text, sizeof(_status_text) - 1);
    _status_text[sizeof(_status_text) - 1] = '\0';
    _last_status_clear = millis();
}

const char *kernel_get_status() { return _status_text; }
AppId kernel_current_app() { return _current_app; }

void kernel_draw_status_bar() {
    unsigned long now = millis();

    if (now - _last_bat_check > 30000 || _bat_pct < 0) {
        _last_bat_check = now;
        BatteryStatus bat = g_power.read_battery();
        _bat_pct = bat.level_percent;
    }

    tft.fillRect(0, 0, DISPLAY_WIDTH, STATUS_BAR_HEIGHT, TFT_BLUE);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.setTextSize(1);

    tft.setCursor(4, 5);
    tft.print(apps[_current_app].icon);
    tft.print(" ");
    tft.print(apps[_current_app].name);

    if (now - _last_status_clear < 3000) {
        tft.setTextColor(TFT_YELLOW, TFT_BLUE);
        tft.setCursor(DISPLAY_WIDTH / 2 - 40, 5);
        tft.print(_status_text);
    }

    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.setCursor(DISPLAY_WIDTH - 52, 5);
    tft.print("Bat:");
    if (_bat_pct < 20) tft.setTextColor(TFT_RED, TFT_BLUE);
    else if (_bat_pct < 50) tft.setTextColor(TFT_YELLOW, TFT_BLUE);
    else tft.setTextColor(TFT_GREEN, TFT_BLUE);
    tft.print(_bat_pct);
    tft.print("%");
}
