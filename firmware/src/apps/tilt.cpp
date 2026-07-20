#include "apps.h"
#include "kernel.h"
#include "config.h"
#include "mpu/mpu.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <math.h>

extern TFT_eSPI tft;
extern MPU6050 g_mpu;

#define CENTER_X (DISPLAY_WIDTH / 2)
#define CENTER_Y (STATUS_BAR_HEIGHT + 130)
#define CIRCLE_R 80

static MPUData _data;
static unsigned long _last_read = 0;
static float _smooth_pitch = 0, _smooth_roll = 0;

void app_tilt_create() {
    memset(&_data, 0, sizeof(_data));
    _smooth_pitch = 0;
    _smooth_roll = 0;
}

void app_tilt_destroy() {}

void app_tilt_focus() {
    tft.fillScreen(TFT_BLACK);
    app_tilt_draw();
}

void app_tilt_key(uint8_t mod, uint8_t key) {
    if (key == 0x29) kernel_run_app(APP_DESKTOP);
}

void app_tilt_tick() {
    unsigned long now = millis();
    if (now - _last_read < 33) return;
    _last_read = now;

    if (!g_mpu.present()) return;

    g_mpu.read(&_data);

    _smooth_pitch = _smooth_pitch * 0.85f + _data.pitch * 0.15f;
    _smooth_roll  = _smooth_roll  * 0.85f + _data.roll  * 0.15f;

    app_tilt_draw();
}

void app_tilt_draw() {
    tft.fillRect(0, STATUS_BAR_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT - STATUS_BAR_HEIGHT, TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(10, STATUS_BAR_HEIGHT + 8);
    tft.print("Tilt Sensor");

    tft.drawCircle(CENTER_X, CENTER_Y, CIRCLE_R, TFT_DARKGREY);
    tft.fillCircle(CENTER_X, CENTER_Y, 3, TFT_DARKGREY);

    float pitch_rad = _smooth_pitch * M_PI / 180.0f;
    float roll_rad  = _smooth_roll  * M_PI / 180.0f;

    int dot_x = CENTER_X + (int)(roll_rad  * 80.0f);
    int dot_y = CENTER_Y + (int)(pitch_rad * 80.0f);

    if (dot_x < CENTER_X - CIRCLE_R) dot_x = CENTER_X - CIRCLE_R;
    if (dot_x > CENTER_X + CIRCLE_R) dot_x = CENTER_X + CIRCLE_R;
    if (dot_y < CENTER_Y - CIRCLE_R) dot_y = CENTER_Y - CIRCLE_R;
    if (dot_y > CENTER_Y + CIRCLE_R) dot_y = CENTER_Y + CIRCLE_R;

    tft.fillCircle(dot_x, dot_y, 8, TFT_CYAN);
    tft.fillCircle(dot_x, dot_y, 4, TFT_WHITE);

    tft.drawLine(CENTER_X - CIRCLE_R, CENTER_Y, CENTER_X + CIRCLE_R, CENTER_Y, TFT_DARKGREY);
    tft.drawLine(CENTER_X, CENTER_Y - CIRCLE_R, CENTER_X, CENTER_Y + CIRCLE_R, TFT_DARKGREY);

    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(CENTER_X + CIRCLE_R + 8, CENTER_Y - 4); tft.print("R");
    tft.setCursor(CENTER_X - CIRCLE_R - 16, CENTER_Y - 4); tft.print("L");
    tft.setCursor(CENTER_X - 4, CENTER_Y - CIRCLE_R - 12); tft.print("F");
    tft.setCursor(CENTER_X - 4, CENTER_Y + CIRCLE_R + 4); tft.print("B");

    if (g_mpu.present()) {
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.setCursor(10, DISPLAY_HEIGHT - 60);
        tft.printf("Accel: X=%4d Y=%4d Z=%4d", _data.ax, _data.ay, _data.az);
        tft.setCursor(10, DISPLAY_HEIGHT - 44);
        tft.printf("Gyro:  X=%4d Y=%4d Z=%4d", _data.gx, _data.gy, _data.gz);
        tft.setCursor(10, DISPLAY_HEIGHT - 28);
        tft.printf("Pitch: %5.1f  Roll: %5.1f", _smooth_pitch, _smooth_roll);
    } else {
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.setCursor(10, DISPLAY_HEIGHT - 28);
        tft.print("MPU6050 not found");
    }

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setCursor(10, DISPLAY_HEIGHT - 12);
    tft.print("ESC:Exit");
}
