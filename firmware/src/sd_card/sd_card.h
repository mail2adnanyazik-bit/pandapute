#pragma once
#include <stdint.h>
#include <SdFat.h>

#define SD_MAX_NAME 36

class SDCard {
public:
    SDCard();
    bool begin();
    bool is_mounted() const;

    uint16_t list_dir(const char* path, char names[][SD_MAX_NAME],
                      bool* is_dir, uint16_t max_entries);

    uint64_t total_bytes() const;
    uint64_t used_bytes();

private:
    SdFat _sd;
    bool _mounted;
};
