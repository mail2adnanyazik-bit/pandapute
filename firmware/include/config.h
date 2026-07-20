#pragma once

// =============================================================================
// Pandapute - Hardware Configuration
// =============================================================================

// --- Display (ILI9341 320x240 via SPI) ---
#define DISPLAY_WIDTH         320
#define DISPLAY_HEIGHT        240
#define DISPLAY_MOSI_PIN      13
#define DISPLAY_SCLK_PIN      12
#define DISPLAY_CS_PIN        10
#define DISPLAY_DC_PIN        11
#define DISPLAY_RST_PIN       14
#define DISPLAY_BL_PIN        15

// --- Touch (XPT2046, shares SPI bus with display) ---
#define TOUCH_CS_PIN          21
#define TOUCH_IRQ_PIN         16

// --- SD Card (shares SPI bus with display) ---
#define SD_CS_PIN             7
#define SD_MISO_PIN           9
#define SD_MOSI_PIN           13
#define SD_SCLK_PIN           12

// Touch
#define TOUCH_MISO_PIN        9

// Shared SPI MISO for touch+SD (tied together on module)
#define SPI_MISO_PIN          9

// --- SPI bus: all three devices on same bus with individual CS ---

// --- Audio (I²S + ES8311 codec) ---
#define I2S_BCLK_PIN          17
#define I2S_LRCK_PIN          18
#define I2S_DIN_PIN           19
#define I2S_DOUT_PIN          20
#define I2S_MCLK_PIN          6

#define AUDIO_I2C_SDA_PIN     5
#define AUDIO_I2C_SCL_PIN     4
#define ES8311_I2C_ADDR       0x18

// --- Power / Battery (IP5306) ---
#define IP5306_I2C_ADDR       0x75
#define POWER_LED_PIN         48
#define CHARGING_LED_PIN      47
#define BAT_ADC_PIN           1
#define BAT_ADC_DIVIDER_R1    100000.0f
#define BAT_ADC_DIVIDER_R2    100000.0f

// --- Keyboard Matrix (TCA8418 I²C) ---
#define TCA8418_I2C_ADDR      0x34
#define KEYBOARD_ROWS         4
#define KEYBOARD_COLS         8

// Direct GPIO matrix (fallback if TCA8418 not used)
static const uint8_t MATRIX_ROW_PINS[KEYBOARD_ROWS] = {26, 27, 28, 29};
static const uint8_t MATRIX_COL_PINS[KEYBOARD_COLS] = {30, 31, 32, 33, 34, 35, 36, 37};

// --- USB (native) ---
#define USB_D_PLUS_PIN        23
#define USB_D_MINUS_PIN       22

// --- UART ---
#define UART_TX_PIN           43
#define UART_RX_PIN           44

// --- Navigation ---
#define NAV_COM_PIN           45

// --- System ---
#define DEBOUNCE_THRESHOLD_MS 5
#define KEYBOARD_POLL_MS      10
#define STATUS_BAR_HEIGHT     20
