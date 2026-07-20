#pragma once
#include <stdint.h>

// USB HID keycodes
#define KEY_A       0x04
#define KEY_B       0x05
#define KEY_C       0x06
#define KEY_D       0x07
#define KEY_E       0x08
#define KEY_F       0x09
#define KEY_G       0x0A
#define KEY_H       0x0B
#define KEY_I       0x0C
#define KEY_J       0x0D
#define KEY_K       0x0E
#define KEY_L       0x0F
#define KEY_M       0x10
#define KEY_N       0x11
#define KEY_O       0x12
#define KEY_P       0x13
#define KEY_Q       0x14
#define KEY_R       0x15
#define KEY_S       0x16
#define KEY_T       0x17
#define KEY_U       0x18
#define KEY_V       0x19
#define KEY_W       0x1A
#define KEY_X       0x1B
#define KEY_Y       0x1C
#define KEY_Z       0x1D

#define KEY_1       0x1E
#define KEY_2       0x1F
#define KEY_3       0x20
#define KEY_4       0x21
#define KEY_5       0x22
#define KEY_6       0x23
#define KEY_7       0x24
#define KEY_8       0x25
#define KEY_9       0x26
#define KEY_0       0x27

#define KEY_RETURN      0x28
#define KEY_ESCAPE      0x29
#define KEY_BACKSPACE   0x2A
#define KEY_TAB         0x2B
#define KEY_SPACE       0x2C
#define KEY_MINUS       0x2D
#define KEY_EQUAL       0x2E
#define KEY_BRACKET_L   0x2F
#define KEY_BRACKET_R   0x30
#define KEY_BACKSLASH   0x31
#define KEY_SEMICOLON   0x33
#define KEY_QUOTE       0x34
#define KEY_BACKTICK    0x35
#define KEY_COMMA       0x36
#define KEY_PERIOD      0x37
#define KEY_FSLASH      0x38
#define KEY_CAPS_LOCK   0x39

#define KEY_F1          0x3A
#define KEY_F2          0x3B
#define KEY_F3          0x3C
#define KEY_F4          0x3D
#define KEY_F5          0x3E
#define KEY_F6          0x3F
#define KEY_F7          0x40
#define KEY_F8          0x41
#define KEY_F9          0x42
#define KEY_F10         0x43
#define KEY_F11         0x44
#define KEY_F12         0x45

#define KEY_INSERT      0x49
#define KEY_HOME        0x4A
#define KEY_PAGE_UP     0x4B
#define KEY_DELETE      0x4C
#define KEY_END         0x4D
#define KEY_PAGE_DOWN   0x4E
#define KEY_RIGHT       0x4F
#define KEY_LEFT        0x50
#define KEY_DOWN        0x51
#define KEY_UP          0x52

#define KEY_NUM_LOCK    0x53
#define KEY_SCROLL_LOCK 0x47
#define KEY_PAUSE       0x48

#define KEY_LCTRL       0xE0
#define KEY_LSHIFT      0xE1
#define KEY_LALT        0xE2
#define KEY_LGUI        0xE3
#define KEY_RCTRL       0xE4
#define KEY_RSHIFT      0xE5
#define KEY_RALT        0xE6
#define KEY_RGUI        0xE7

// Keymap: [row][col] -> USB HID keycode
// Modify this to match your physical key layout.
// Row 0 = top row (closest to display)

static const uint8_t KEYMAP[KEYBOARD_ROWS][KEYBOARD_COLS] = {
    //  col0    col1    col2    col3    col4    col5    col6    col7
    {  KEY_1,  KEY_2,  KEY_3,  KEY_4,  KEY_5,  KEY_6,  KEY_7,  KEY_8   },  // row 0
    {  KEY_9,  KEY_0, KEY_MINUS, KEY_EQUAL, KEY_BACKSPACE, KEY_TAB, KEY_Q, KEY_W },  // row 1
    {  KEY_E,  KEY_R,  KEY_T,  KEY_Y,  KEY_U,  KEY_I,  KEY_O,  KEY_P   },  // row 2
    {  KEY_BRACKET_L, KEY_BRACKET_R, KEY_BACKSLASH, KEY_CAPS_LOCK, KEY_A, KEY_S, KEY_D, KEY_F },  // row 3
};

// Layer support — define as needed:
// static const uint8_t KEYMAP_FN[4][8] = { ... };
// static const uint8_t KEYMAP_SYM[4][8] = { ... };
