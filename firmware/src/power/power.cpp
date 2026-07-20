#include "power.h"
#include "config.h"
#include <Arduino.h>
#include <Wire.h>

PowerManager::PowerManager() {}

bool PowerManager::begin() {
    Wire.begin();
    pinMode(POWER_LED_PIN, OUTPUT);
    pinMode(CHARGING_LED_PIN, OUTPUT);
    digitalWrite(POWER_LED_PIN, HIGH);
    digitalWrite(CHARGING_LED_PIN, LOW);
    return true;
}

BatteryStatus PowerManager::read_battery() {
    BatteryStatus status = {0, false, false, false};

    uint8_t reg0x78 = _read_ip5306_reg(0x78);
    if (reg0x78 == 0xFF) {
        status.level_percent = read_battery_adc();
        return status;
    }

    status.charging = (reg0x78 & 0x08) != 0;
    status.charged  = (reg0x78 & 0x20) != 0;

    uint8_t reg0x71 = _read_ip5306_reg(0x71);
    uint8_t raw_level = reg0x71 & 0x0F;

    switch (raw_level) {
        case 0:  status.level_percent = 0;   break;
        case 1:  status.level_percent = 20;  break;
        case 2:  status.level_percent = 40;  break;
        case 3:  status.level_percent = 60;  break;
        case 4:  status.level_percent = 80;  break;
        case 5:  status.level_percent = 100; break;
        default: status.level_percent = read_battery_adc(); break;
    }

    return status;
}

uint8_t PowerManager::read_battery_adc() {
    analogReadResolution(12);
    int raw = analogRead(BAT_ADC_PIN);
    float voltage = (float)raw / 4095.0f * 3.3f;
    float battery = voltage * (BAT_ADC_DIVIDER_R1 + BAT_ADC_DIVIDER_R2) / BAT_ADC_DIVIDER_R2;
    if (battery > 4.2f) battery = 4.2f;
    if (battery < 3.0f) battery = 3.0f;
    float pct = (battery - 3.0f) / (4.2f - 3.0f) * 100.0f;
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;
    return (uint8_t)pct;
}

uint8_t PowerManager::_read_ip5306_reg(uint8_t reg) {
    Wire.beginTransmission(IP5306_I2C_ADDR);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) return 0xFF;
    if (Wire.requestFrom((int)IP5306_I2C_ADDR, (int)1) != 1) return 0xFF;
    return Wire.read();
}

void PowerManager::_write_ip5306_reg(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(IP5306_I2C_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

void PowerManager::set_power_led(bool on) {
    digitalWrite(POWER_LED_PIN, on ? HIGH : LOW);
}

void PowerManager::set_charging_led(bool on) {
    digitalWrite(CHARGING_LED_PIN, on ? HIGH : LOW);
}
