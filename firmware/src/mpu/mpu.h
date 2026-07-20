#pragma once
#include <stdint.h>

#define MPU6050_I2C_ADDR 0x68

struct MPUData {
    int16_t ax, ay, az;   // accelerometer
    int16_t gx, gy, gz;   // gyroscope
    int16_t temp;         // temperature
    float   pitch, roll;  // calculated orientation in degrees
};

class MPU6050 {
public:
    MPU6050();
    bool begin();
    bool read(MPUData* data);
    bool present() const;

private:
    bool _present;
    void _write_reg(uint8_t reg, uint8_t val);
    uint8_t _read_reg(uint8_t reg);
    int16_t _read_word(uint8_t reg);
};
