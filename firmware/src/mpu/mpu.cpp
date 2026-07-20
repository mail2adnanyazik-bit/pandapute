#include "mpu.h"
#include <Arduino.h>
#include <Wire.h>
#include <math.h>

MPU6050::MPU6050() : _present(false) {}

bool MPU6050::begin() {
    Wire.beginTransmission(MPU6050_I2C_ADDR);
    if (Wire.endTransmission() != 0) {
        _present = false;
        return false;
    }

    _write_reg(0x6B, 0x00); // wake up
    _write_reg(0x1C, 0x00); // accel ±2g
    _write_reg(0x1B, 0x00); // gyro ±250°/s
    _write_reg(0x19, 0x00); // max sample rate

    _present = true;
    return true;
}

bool MPU6050::present() const { return _present; }

bool MPU6050::read(MPUData* data) {
    if (!_present || !data) return false;

    // Read all 14 registers in one burst (0x3B to 0x48)
    Wire.beginTransmission(MPU6050_I2C_ADDR);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050_I2C_ADDR, (uint8_t)14);
    if (Wire.available() < 14) return false;

    data->ax = (Wire.read() << 8) | Wire.read();
    data->ay = (Wire.read() << 8) | Wire.read();
    data->az = (Wire.read() << 8) | Wire.read();
    data->temp = (Wire.read() << 8) | Wire.read();
    data->gx = (Wire.read() << 8) | Wire.read();
    data->gy = (Wire.read() << 8) | Wire.read();
    data->gz = (Wire.read() << 8) | Wire.read();

    // Pitch & roll from accelerometer
    float fax = data->ax / 16384.0f;
    float fay = data->ay / 16384.0f;
    float faz = data->az / 16384.0f;

    data->roll  = atan2f(fay, faz) * 180.0f / M_PI;
    data->pitch = atan2f(-fax, sqrtf(fay * fay + faz * faz)) * 180.0f / M_PI;

    return true;
}

void MPU6050::_write_reg(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(MPU6050_I2C_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}
