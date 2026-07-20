#include "keyboard.h"
#include "config.h"
#include <Arduino.h>
#include <string.h>

Keyboard::Keyboard() {
    memset(_raw_matrix, 0, sizeof(_raw_matrix));
    memset(_debounced_matrix, 0, sizeof(_debounced_matrix));
    memset(_previous_matrix, 0, sizeof(_previous_matrix));
    memset(_debounce_counters, 0, sizeof(_debounce_counters));
    _last_scan_us = 0;
}

void Keyboard::begin() {
    for (int c = 0; c < KEYBOARD_COLS; c++) {
        pinMode(MATRIX_COL_PINS[c], INPUT_PULLUP);
    }
    for (int r = 0; r < KEYBOARD_ROWS; r++) {
        pinMode(MATRIX_ROW_PINS[r], OUTPUT);
        digitalWrite(MATRIX_ROW_PINS[r], HIGH);
    }
}

void Keyboard::_deselect_all_rows() {
    for (int r = 0; r < KEYBOARD_ROWS; r++) {
        digitalWrite(MATRIX_ROW_PINS[r], HIGH);
    }
    delayMicroseconds(5);
}

void Keyboard::_select_row(uint8_t row) {
    digitalWrite(MATRIX_ROW_PINS[row], LOW);
    delayMicroseconds(5);
}

void Keyboard::_read_columns(uint8_t row) {
    for (int c = 0; c < KEYBOARD_COLS; c++) {
        _raw_matrix[row][c] = (digitalRead(MATRIX_COL_PINS[c]) == LOW) ? 1 : 0;
    }
}

void Keyboard::scan() {
    unsigned long now = micros();
    if (now - _last_scan_us < 1000) return;
    _last_scan_us = now;

    memcpy(_previous_matrix, _debounced_matrix, sizeof(_previous_matrix));

    for (int r = 0; r < KEYBOARD_ROWS; r++) {
        _deselect_all_rows();
        _select_row(r);
        _read_columns(r);
    }
    _deselect_all_rows();

    for (int r = 0; r < KEYBOARD_ROWS; r++) {
        for (int c = 0; c < KEYBOARD_COLS; c++) {
            uint8_t current = _raw_matrix[r][c];
            if (current == _debounced_matrix[r][c]) {
                _debounce_counters[r][c] = 0;
            } else {
                _debounce_counters[r][c]++;
                if (_debounce_counters[r][c] >= DEBOUNCE_THRESHOLD_MS) {
                    _debounced_matrix[r][c] = current;
                    _debounce_counters[r][c] = 0;
                }
            }
        }
    }
}

bool Keyboard::key_down(uint8_t row, uint8_t col) const {
    if (row >= KEYBOARD_ROWS || col >= KEYBOARD_COLS) return false;
    return _debounced_matrix[row][col];
}

bool Keyboard::key_just_pressed(uint8_t row, uint8_t col) const {
    if (row >= KEYBOARD_ROWS || col >= KEYBOARD_COLS) return false;
    return _debounced_matrix[row][col] && !_previous_matrix[row][col];
}

bool Keyboard::key_just_released(uint8_t row, uint8_t col) const {
    if (row >= KEYBOARD_ROWS || col >= KEYBOARD_COLS) return false;
    return !_debounced_matrix[row][col] && _previous_matrix[row][col];
}

bool Keyboard::any_key_down() const {
    for (int r = 0; r < KEYBOARD_ROWS; r++)
        for (int c = 0; c < KEYBOARD_COLS; c++)
            if (_debounced_matrix[r][c]) return true;
    return false;
}

const uint8_t (&Keyboard::debounced_matrix() const)[KEYBOARD_ROWS][KEYBOARD_COLS] {
    return _debounced_matrix;
}
