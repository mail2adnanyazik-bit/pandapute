#include "audio.h"
#include "config.h"
#include <Arduino.h>
#include <Wire.h>
#include <I2S.h>
#include <math.h>

Audio::Audio() : _volume(50), _i2s_initialized(false), _codec_present(false) {}

bool Audio::begin() {
    Wire.begin(AUDIO_I2C_SDA_PIN, AUDIO_I2C_SCL_PIN);

    _codec_present = _init_codec();
    _i2s_initialized = _init_i2s();

    if (_i2s_initialized) {
        Serial.println(_codec_present ? "Audio: codec + i2s ok"
                                      : "Audio: i2s ok, no codec");
        return true;
    }

    Serial.println("Audio: i2s init failed");
    return false;
}

bool Audio::_init_i2s() {
    if (!I2S.begin(I2S_PHILIPS_MODE, 44100, 16)) return false;

    I2S.setAllPins(I2S_BCLK_PIN, I2S_LRCK_PIN, I2S_DIN_PIN, I2S_DOUT_PIN, I2S_DOUT_PIN);
    I2S.setDuplex();
    I2S.setBufferSize(256);
    return true;
}

bool Audio::_init_codec() {
    Wire.beginTransmission(ES8311_I2C_ADDR);
    if (Wire.endTransmission() != 0) return false;

    _i2c_write(0x00, 0x00);  // soft reset
    delay(10);
    _i2c_write(0x06, 0x00);  // MCLK / 2
    _i2c_write(0x04, 0x50);  // DAC + ADC enabled
    _i2c_write(0x10, 0x04);  // 44.1 kHz
    _i2c_write(0x14, 0x24);  // DAC unmuted, output on
    _i2c_write(0x15, 0x50);  // DAC volume ~ -10 dB
    _i2c_write(0x18, 0x30);  // ADC mic gain
    _i2c_write(0x1A, 0x0A);  // ADC input = analog in
    _i2c_write(0x1C, 0x00);  // ADC unmuted
    return true;
}

void Audio::_i2c_write(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(ES8311_I2C_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

void Audio::set_volume(uint8_t vol) {
    _volume = (vol > 100) ? 100 : vol;
    if (!_codec_present) return;

    uint8_t reg = map(_volume, 0, 100, 0x38, 0x10);
    _i2c_write(0x15, reg);
}

uint8_t Audio::volume() const { return _volume; }

bool Audio::play_tone(uint16_t freq_hz, uint32_t duration_ms) {
    if (!_i2s_initialized) return false;

    size_t total = 44100 * duration_ms / 1000;
    size_t chunk = 128;
    int16_t buf[chunk * 2];

    for (size_t written = 0; written < total; written += chunk) {
        size_t n = (written + chunk > total) ? total - written : chunk;
        for (size_t i = 0; i < n; i++) {
            float t = (float)(written + i) / 44100.0f;
            float s = sinf(2.0f * M_PI * freq_hz * t) * 0.3f * (_volume / 100.0f);
            int16_t v = (int16_t)(s * 32767.0f);
            buf[i * 2] = v;
            buf[i * 2 + 1] = v;
        }
        I2S.write((const uint8_t*)buf, n * sizeof(int16_t) * 2);
    }
    return true;
}

int Audio::read_mic(int16_t* buffer, size_t count) {
    if (!_i2s_initialized || !_codec_present) return 0;
    return I2S.read(buffer, count * sizeof(int16_t)) / sizeof(int16_t);
}

float Audio::get_mic_level() {
    if (!_i2s_initialized || !_codec_present) return 0.0f;

    int16_t buf[64];
    int read = read_mic(buf, 64);
    if (read <= 0) return 0.0f;

    float sum = 0;
    for (int i = 0; i < read; i++) {
        float s = (float)buf[i] / 32767.0f;
        sum += s * s;
    }
    return sqrtf(sum / read);
}

void Audio::stop() {
    I2S.flush();
}
