#pragma once
#include <stdint.h>

void app_desktop_create();
void app_desktop_destroy();
void app_desktop_focus();
void app_desktop_key(uint8_t modifiers, uint8_t keycode);
void app_desktop_tick();
void app_desktop_draw();

void app_terminal_create();
void app_terminal_destroy();
void app_terminal_focus();
void app_terminal_key(uint8_t modifiers, uint8_t keycode);
void app_terminal_tick();
void app_terminal_draw();

void app_editor_create();
void app_editor_destroy();
void app_editor_focus();
void app_editor_key(uint8_t modifiers, uint8_t keycode);
void app_editor_tick();
void app_editor_draw();

void app_files_create();
void app_files_destroy();
void app_files_focus();
void app_files_key(uint8_t modifiers, uint8_t keycode);
void app_files_tick();
void app_files_draw();

void app_settings_create();
void app_settings_destroy();
void app_settings_focus();
void app_settings_key(uint8_t modifiers, uint8_t keycode);
void app_settings_tick();
void app_settings_draw();

void app_piano_create();
void app_piano_destroy();
void app_piano_focus();
void app_piano_key(uint8_t modifiers, uint8_t keycode);
void app_piano_tick();
void app_piano_draw();

void app_mic_create();
void app_mic_destroy();
void app_mic_focus();
void app_mic_key(uint8_t modifiers, uint8_t keycode);
void app_mic_tick();
void app_mic_draw();

void app_tilt_create();
void app_tilt_destroy();
void app_tilt_focus();
void app_tilt_key(uint8_t modifiers, uint8_t keycode);
void app_tilt_tick();
void app_tilt_draw();
