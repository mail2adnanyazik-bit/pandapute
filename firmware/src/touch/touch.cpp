#include "touch.h"
#include "config.h"
#include <Arduino.h>
#include <SPI.h>

Touch::Touch() : _calibrated(false) {
    _cal[0] = 200;  _cal[1] = 3800;
    _cal[2] = 200;  _cal[3] = 3800;
    _cal[4] = 0;    _cal[5] = 0;
}

bool Touch::begin() {
    pinMode(TOUCH_CS_PIN, OUTPUT);
    digitalWrite(TOUCH_CS_PIN, HIGH);
    pinMode(TOUCH_IRQ_PIN, INPUT_PULLUP);
    delay(10);
    return true;
}

bool Touch::_spi_transfer16(uint8_t cmd, uint16_t &data) {
    digitalWrite(TOUCH_CS_PIN, LOW);
    SPI.transfer(cmd);
    delayMicroseconds(3);
    data = SPI.transfer16(0);
    digitalWrite(TOUCH_CS_PIN, HIGH);
    return true;
}

TouchPoint Touch::read() {
    TouchPoint pt = {0, 0, 0, false};

    uint16_t raw_x, raw_y, raw_z1, raw_z2;

    digitalWrite(TOUCH_CS_PIN, LOW);
    SPI.transfer(0x90);
    delayMicroseconds(3);
    raw_x = SPI.transfer16(0);
    SPI.transfer(0xD0);
    delayMicroseconds(3);
    raw_y = SPI.transfer16(0);
    SPI.transfer(0xB0);
    delayMicroseconds(3);
    raw_z1 = SPI.transfer16(0);
    SPI.transfer(0xC0);
    delayMicroseconds(3);
    raw_z2 = SPI.transfer16(0);
    digitalWrite(TOUCH_CS_PIN, HIGH);

    raw_x >>= 3;
    raw_y >>= 3;
    raw_z1 >>= 3;
    raw_z2 >>= 3;

    uint16_t z = raw_z1 + 4095 - raw_z2;
    if (z < 100 || raw_x < 100 || raw_y < 100 || raw_x > 4095 || raw_y > 4095) return pt;

    pt.x = raw_x;
    pt.y = raw_y;
    pt.z = z;

    if (_calibrated) _map_to_display(pt.x, pt.y);
    pt.valid = true;
    return pt;
}

void Touch::_map_to_display(uint16_t &x, uint16_t &y) {
    int32_t mx = map((int32_t)x, _cal[0], _cal[1], 0, DISPLAY_WIDTH);
    int32_t my = map((int32_t)y, _cal[2], _cal[3], 0, DISPLAY_HEIGHT);
    if (mx < 0) mx = 0;
    if (mx >= DISPLAY_WIDTH) mx = DISPLAY_WIDTH - 1;
    if (my < 0) my = 0;
    if (my >= DISPLAY_HEIGHT) my = DISPLAY_HEIGHT - 1;
    x = (uint16_t)mx;
    y = (uint16_t)my;
}

void Touch::calibrate(uint16_t cal[6]) {
    for (int i = 0; i < 6; i++) _cal[i] = cal[i];
    _calibrated = true;
}

bool Touch::calibrated() const { return _calibrated; }
