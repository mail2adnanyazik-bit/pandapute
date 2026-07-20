#pragma once
#include <stdint.h>

struct BatteryStatus {
    uint8_t level_percent;
    bool    charging;
    bool    charged;
    bool    usb_connected;
};

class PowerManager {
public:
    PowerManager();
    bool begin();
    BatteryStatus read_battery();
    uint8_t read_battery_adc();
    void set_power_led(bool on);
    void set_charging_led(bool on);

private:
    uint8_t _read_ip5306_reg(uint8_t reg);
    void    _write_ip5306_reg(uint8_t reg, uint8_t val);
};
