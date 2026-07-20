#include "sd_card.h"
#include "config.h"
#include <Arduino.h>
#include <SPI.h>

SDCard::SDCard() : _mounted(false) {}

bool SDCard::begin() {
    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);

    if (!_sd.begin(SdSpiConfig(SD_CS_PIN, SHARED_SPI, SD_SCK_MHZ(25)))) {
        Serial.println("SD card init failed");
        return false;
    }

    _mounted = true;
    return true;
}

bool SDCard::is_mounted() const { return _mounted; }

bool SDCard::open_file(const char* path, oflag_t flags) {
    if (!_mounted) return false;
    return _file.open(path, flags);
}

void SDCard::close_file() { _file.close(); }

bool SDCard::read_line(char* buffer, size_t max_len) {
    if (!_mounted || !_file) return false;
    return _file.fgets(buffer, max_len) > 0;
}

bool SDCard::write(const uint8_t* data, size_t len) {
    if (!_mounted || !_file) return false;
    return _file.write(data, len) == len;
}

uint64_t SDCard::total_bytes() const {
    if (!_mounted) return 0;
    return (uint64_t)_sd.clusterCount() * _sd.bytesPerCluster();
}

uint64_t SDCard::used_bytes() {
    if (!_mounted) return 0;
    return total_bytes() - (uint64_t)_sd.freeClusterCount() * _sd.bytesPerCluster();
}
