#pragma once
#include <stdint.h>

void app_desktop_init();
void app_desktop_key(uint8_t modifiers, uint8_t keycode);

void app_terminal_init();
void app_terminal_key(uint8_t modifiers, uint8_t keycode);
void app_terminal_tick();

void app_editor_init();
void app_editor_key(uint8_t modifiers, uint8_t keycode);
void app_editor_tick();

void app_files_init();
void app_files_key(uint8_t modifiers, uint8_t keycode);

void app_settings_init();
void app_settings_key(uint8_t modifiers, uint8_t keycode);

void app_piano_init();
void app_piano_key(uint8_t modifiers, uint8_t keycode);
void app_piano_tick();

void app_mic_init();
void app_mic_key(uint8_t modifiers, uint8_t keycode);
void app_mic_tick();

void app_tilt_init();
void app_tilt_key(uint8_t modifiers, uint8_t keycode);
void app_tilt_tick();
