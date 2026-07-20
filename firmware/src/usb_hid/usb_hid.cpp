#include "usb_hid.h"
#include <USB.h>
#include <USBHIDKeyboard.h>

static USBHIDKeyboard HIDKbd;

// Map our HID keycodes (USB HID usage IDs) to arduino-esp32 KEY_ constants
static uint8_t _map_hid_to_arduino(uint8_t keycode) {
    // Modifier keys
    if (keycode == 0xE1) return KEY_LEFT_SHIFT;
    if (keycode == 0xE0) return KEY_LEFT_CTRL;
    if (keycode == 0xE2) return KEY_LEFT_ALT;
    if (keycode == 0xE3) return KEY_LEFT_GUI;
    if (keycode == 0xE5) return KEY_RIGHT_SHIFT;
    if (keycode == 0xE4) return KEY_RIGHT_CTRL;
    if (keycode == 0xE6) return KEY_RIGHT_ALT;
    if (keycode == 0xE7) return KEY_RIGHT_GUI;

    // Arrow keys
    if (keycode == 0x52) return KEY_UP_ARROW;
    if (keycode == 0x51) return KEY_DOWN_ARROW;
    if (keycode == 0x50) return KEY_LEFT_ARROW;
    if (keycode == 0x4F) return KEY_RIGHT_ARROW;

    // Special keys
    if (keycode == 0x28) return KEY_RETURN;
    if (keycode == 0x29) return KEY_ESC;
    if (keycode == 0x2A) return KEY_BACKSPACE;
    if (keycode == 0x2B) return KEY_TAB;
    if (keycode == 0x2C) return ' ';
    if (keycode == 0x39) return KEY_CAPS_LOCK;
    if (keycode == 0x4C) return KEY_DELETE;
    if (keycode == 0x4A) return KEY_HOME;
    if (keycode == 0x4D) return KEY_END;
    if (keycode == 0x49) return KEY_INSERT;
    if (keycode == 0x4B) return KEY_PAGE_UP;
    if (keycode == 0x4E) return KEY_PAGE_DOWN;

    // F-keys
    if (keycode >= 0x3A && keycode <= 0x45) return 0xC2 + (keycode - 0x3A);

    // Letters: HID 0x04-0x1D -> a-z
    if (keycode >= 0x04 && keycode <= 0x1D) return keycode;

    // Numbers: HID 0x1E-0x27 -> '1'-'0'
    if (keycode >= 0x1E && keycode <= 0x26) return '1' + (keycode - 0x1E);
    if (keycode == 0x27) return '0';

    return keycode;
}

HIDKeyboard::HIDKeyboard() : _connected(false) {}

void HIDKeyboard::begin() {
    HIDKbd.begin();
    delay(200);
    _connected = true;
}

bool HIDKeyboard::is_connected() const {
    return _connected;
}

void HIDKeyboard::send_key(uint8_t modifiers, uint8_t keycode) {
    if (!_connected) return;

    // Press modifier keys
    if (modifiers & 0x02) HIDKbd.press(KEY_LEFT_SHIFT);
    if (modifiers & 0x04) HIDKbd.press(KEY_LEFT_CTRL);
    if (modifiers & 0x08) HIDKbd.press(KEY_LEFT_ALT);
    if (modifiers & 0x10) HIDKbd.press(KEY_LEFT_GUI);

    // Press the actual key
    uint8_t mapped = _map_hid_to_arduino(keycode);
    if (mapped != KEY_LEFT_SHIFT && mapped != KEY_LEFT_CTRL &&
        mapped != KEY_LEFT_ALT && mapped != KEY_LEFT_GUI &&
        mapped != KEY_RIGHT_SHIFT && mapped != KEY_RIGHT_CTRL &&
        mapped != KEY_RIGHT_ALT && mapped != KEY_RIGHT_GUI) {
        HIDKbd.press(mapped);
    }

    delay(5);
    HIDKbd.releaseAll();
}

void HIDKeyboard::send_string(const char *str) {
    if (!_connected || !str) return;
    while (*str) {
        HIDKbd.write(*str);
        delay(5);
        str++;
    }
}

void HIDKeyboard::release_all() {
    HIDKbd.releaseAll();
}
