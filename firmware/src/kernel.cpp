#include "kernel.h"
#include "apps.h"
#include "config.h"
#include "keyboard/keymap.h"
#include "power/power.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;
extern PowerManager g_power;

typedef struct {
    const char* name;
    const char* icon;
} AppInfo;

static const AppInfo _apps[APP_COUNT] = {
    { "Desktop",  "~" },
    { "Terminal", ">" },
    { "Editor",   "E" },
    { "Files",    "F" },
    { "Settings", "S" },
    { "Piano",    "P" },
    { "Mic",      "M" },
    { "Tilt",     "T" },
};

static AppId _current = APP_DESKTOP;
static char _status_text[32] = "Ready";
static unsigned long _status_until = 0;
static unsigned long _last_bat_check = 0;
static int _bat_pct = -1;

const char* kernel_app_name(AppId id) { return _apps[id].name; }
const char* kernel_app_icon(AppId id) { return _apps[id].icon; }
AppId kernel_current() { return _current; }

void kernel_init() {
    kernel_open(APP_DESKTOP);
}

void kernel_open(AppId id) {
    _current = id;
    switch (id) {
        case APP_DESKTOP:   app_desktop_init();   break;
        case APP_TERMINAL:  app_terminal_init();  break;
        case APP_EDITOR:    app_editor_init();    break;
        case APP_FILES:     app_files_init();     break;
        case APP_SETTINGS:  app_settings_init();  break;
        case APP_PIANO:     app_piano_init();     break;
        case APP_MIC:       app_mic_init();       break;
        case APP_TILT:      app_tilt_init();      break;
        default:            break;
    }
}

void kernel_key(uint8_t mod, uint8_t key) {
    switch (_current) {
        case APP_DESKTOP:   app_desktop_key(mod, key);   break;
        case APP_TERMINAL:  app_terminal_key(mod, key);  break;
        case APP_EDITOR:    app_editor_key(mod, key);    break;
        case APP_FILES:     app_files_key(mod, key);     break;
        case APP_SETTINGS:  app_settings_key(mod, key);  break;
        case APP_PIANO:     app_piano_key(mod, key);     break;
        case APP_MIC:       app_mic_key(mod, key);       break;
        case APP_TILT:      app_tilt_key(mod, key);      break;
        default:            break;
    }
}

void kernel_tick() {
    switch (_current) {
        case APP_TERMINAL:  app_terminal_tick();  break;
        case APP_EDITOR:    app_editor_tick();    break;
        case APP_PIANO:     app_piano_tick();     break;
        case APP_MIC:       app_mic_tick();       break;
        case APP_TILT:      app_tilt_tick();      break;
        default:            break;
    }
}

void kernel_set_status(const char* text) {
    strncpy(_status_text, text, sizeof(_status_text) - 1);
    _status_text[sizeof(_status_text) - 1] = '\0';
    _status_until = millis();
}

void kernel_draw_status_bar() {
    unsigned long now = millis();

    if (now - _last_bat_check > 30000 || _bat_pct < 0) {
        _last_bat_check = now;
        _bat_pct = g_power.read_battery().level_percent;
    }

    tft.fillRect(0, 0, DISPLAY_WIDTH, STATUS_BAR_HEIGHT, TFT_BLUE);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.setTextSize(1);

    tft.setCursor(4, 5);
    tft.print(kernel_app_icon(_current));
    tft.print(" ");
    tft.print(kernel_app_name(_current));

    if (now - _status_until < 3000) {
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
