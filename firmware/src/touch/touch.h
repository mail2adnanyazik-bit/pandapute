#pragma once
#include <stdint.h>

struct TouchPoint {
    uint16_t x;
    uint16_t y;
    uint16_t z;
    bool     valid;
};

class Touch {
public:
    Touch();
    bool begin();
    TouchPoint read();
    void calibrate(uint16_t cal[6]);
    bool calibrated() const;

private:
    uint16_t _cal[6];
    bool     _calibrated;
    bool     _spi_transfer16(uint8_t cmd, uint16_t &data);
    void     _map_to_display(uint16_t &x, uint16_t &y);
};
