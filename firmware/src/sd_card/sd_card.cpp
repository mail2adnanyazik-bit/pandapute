#include "sd_card.h"
#include "config.h"
#include <Arduino.h>
#include <SPI.h>

SDCard::SDCard() : _mounted(false) {}

bool SDCard::begin() {
    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);

    if (!_sd.begin(SdSpiConfig(SD_CS_PIN, SHARED_SPI, SD_SCK_MHZ(25)))) {
        Serial.println("SD: init failed");
        return false;
    }

    _mounted = true;
    return true;
}

bool SDCard::is_mounted() const { return _mounted; }

uint16_t SDCard::list_dir(const char* path, char names[][SD_MAX_NAME],
                          bool* is_dir, uint16_t max_entries) {
    if (!_mounted) return 0;

    FsFile dir;
    if (!dir.open(path, O_READ)) return 0;

    uint16_t count = 0;
    FsFile entry;
    while (count < max_entries && entry.openNext(&dir, O_READ)) {
        if (entry.isHidden()) {
            entry.close();
            continue;
        }
        entry.getName(names[count], SD_MAX_NAME);
        is_dir[count] = entry.isDirectory();
        count++;
        entry.close();
    }
    dir.close();
    return count;
}

uint64_t SDCard::total_bytes() const {
    if (!_mounted) return 0;
    return (uint64_t)_sd.clusterCount() * _sd.bytesPerCluster();
}

uint64_t SDCard::used_bytes() {
    if (!_mounted) return 0;
    return total_bytes() - (uint64_t)_sd.freeClusterCount() * _sd.bytesPerCluster();
}
