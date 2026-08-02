#pragma once
#include <stdint.h>
#include <stddef.h>

class Audio {
public:
    Audio();
    bool begin();
    void set_volume(uint8_t vol);
    uint8_t volume() const;

    bool play_tone(uint16_t freq_hz, uint32_t duration_ms);
    int  read_mic(int16_t* buffer, size_t count);
    float get_mic_level();
    void stop();

private:
    uint8_t _volume;
    bool _i2s_initialized;
    bool _codec_present;

    bool _init_i2s();
    bool _init_codec();
    void _i2c_write(uint8_t reg, uint8_t val);
};
