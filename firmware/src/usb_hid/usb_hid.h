#pragma once
#include <stdint.h>
#include <Arduino.h>

class HIDKeyboard {
public:
    HIDKeyboard();
    void begin();
    bool is_connected() const;
    void send_key(uint8_t modifiers, uint8_t keycode);
    void send_string(const char *str);
    void release_all();

private:
    bool _connected;
};
