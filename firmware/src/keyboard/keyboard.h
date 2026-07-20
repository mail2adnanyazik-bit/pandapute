#pragma once
#include <stdint.h>

#define KEYBOARD_ROWS 4
#define KEYBOARD_COLS 8

class Keyboard {
public:
    Keyboard();

    void begin();
    void scan();

    bool key_down(uint8_t row, uint8_t col) const;
    bool key_just_pressed(uint8_t row, uint8_t col) const;
    bool key_just_released(uint8_t row, uint8_t col) const;
    bool any_key_down() const;

    const uint8_t (&debounced_matrix() const)[KEYBOARD_ROWS][KEYBOARD_COLS];

private:
    uint8_t _raw_matrix[KEYBOARD_ROWS][KEYBOARD_COLS];
    uint8_t _debounced_matrix[KEYBOARD_ROWS][KEYBOARD_COLS];
    uint8_t _previous_matrix[KEYBOARD_ROWS][KEYBOARD_COLS];

    unsigned long _last_scan_us;
    uint8_t       _debounce_counters[KEYBOARD_ROWS][KEYBOARD_COLS];

    void _select_row(uint8_t row);
    void _read_columns(uint8_t row);
    void _deselect_all_rows();
};
