#include "audio.h"
#include "config.h"
#include <Arduino.h>
#include <Wire.h>
#include <I2S.h>
#include <esp_log.h>

static const char* TAG = "Audio";

Audio::Audio() : _volume(50), _i2s_initialized(false), _codec_present(false) {}

bool Audio::begin() {
    Wire.begin(AUDIO_I2C_SDA_PIN, AUDIO_I2C_SCL_PIN);

    _codec_present = _init_codec();
    _i2s_initialized = _init_i2s();

    if (_i2s_initialized) {
        ESP_LOGI(TAG, "I2S OK");
        if (_codec_present) ESP_LOGI(TAG, "ES8311 OK");
        else ESP_LOGW(TAG, "No codec — speaker/mic unavailable");
        return true;
    }

    ESP_LOGE(TAG, "I2S init failed");
    return false;
}

bool Audio::_init_i2s() {
    // Use the built-in Arduino I2S library
    // Philips mode, 44100 Hz, 16-bit, stereo
    if (!I2S.begin(I2S_PHILIPS_MODE, 44100, 16)) {
        return false;
    }

    // Set pins for ESP32-S3
    I2S.setAllPins(I2S_BCLK_PIN, I2S_LRCK_PIN, I2S_DIN_PIN, I2S_DOUT_PIN, I2S_DOUT_PIN);
    I2S.setDuplex();
    I2S.setBufferSize(256);

    return true;
}

bool Audio::_init_codec() {
    Wire.beginTransmission(ES8311_I2C_ADDR);
    if (Wire.endTransmission() != 0) {
        return false;
    }

    // Soft reset
    Wire.beginTransmission(ES8311_I2C_ADDR);
    Wire.write(0x00); Wire.write(0x00);
    Wire.endTransmission();
    delay(10);

    // Clock config: MCLK / 2
    Wire.beginTransmission(ES8311_I2C_ADDR);
    Wire.write(0x06); Wire.write(0x00);
    Wire.endTransmission();

    // Enable DAC & ADC
    Wire.beginTransmission(ES8311_I2C_ADDR);
    Wire.write(0x04); Wire.write(0x50);
    Wire.endTransmission();

    // Sample rate: 44.1kHz
    Wire.beginTransmission(ES8311_I2C_ADDR);
    Wire.write(0x10); Wire.write(0x04);
    Wire.endTransmission();

    // Unmute DAC, enable output
    Wire.beginTransmission(ES8311_I2C_ADDR);
    Wire.write(0x14); Wire.write(0x24);
    Wire.endTransmission();

    // Set DAC volume (0x50 = ~-10dB)
    Wire.beginTransmission(ES8311_I2C_ADDR);
    Wire.write(0x15); Wire.write(0x50);
    Wire.endTransmission();

    // ADC volume (mic gain)
    Wire.beginTransmission(ES8311_I2C_ADDR);
    Wire.write(0x18); Wire.write(0x30);
    Wire.endTransmission();

    // ADC input select: analog input
    Wire.beginTransmission(ES8311_I2C_ADDR);
    Wire.write(0x1A); Wire.write(0x0A);
    Wire.endTransmission();

    // Unmute ADC
    Wire.beginTransmission(ES8311_I2C_ADDR);
    Wire.write(0x1C); Wire.write(0x00);
    Wire.endTransmission();

    return true;
}

void Audio::set_volume(uint8_t vol) {
    _volume = (vol > 100) ? 100 : vol;
    if (!_codec_present) return;

    // Map 0-100 to ES8311 DAC volume register (0x38 = mute, 0x00 = max)
    uint8_t reg = map(_volume, 0, 100, 0x38, 0x10);
    Wire.beginTransmission(ES8311_I2C_ADDR);
    Wire.write(0x15); Wire.write(reg);
    Wire.endTransmission();
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

bool Audio::play_wav(const uint8_t* data, size_t len) {
    if (!_i2s_initialized || !data || len < 44) return false;

    uint32_t data_start = data[10] | (data[11] << 8) | (data[12] << 16) | (data[13] << 24);
    const int16_t* samples = (const int16_t*)(data + data_start);
    size_t sample_count = (len - data_start) / 2;

    size_t chunk = 256;
    size_t written = 0;
    while (written < sample_count) {
        size_t n = (written + chunk > sample_count) ? sample_count - written : chunk;
        I2S.write((const uint8_t*)(samples + written), n * sizeof(int16_t));
        written += n;
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
