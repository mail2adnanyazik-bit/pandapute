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
    bool play_wav(const uint8_t* data, size_t len);
    int  read_mic(int16_t* buffer, size_t count);
    float get_mic_level();
    void stop();

private:
    uint8_t _volume;
    bool _i2s_initialized;

    bool _init_i2s();
    bool _init_codec();
    bool _codec_present;
};
