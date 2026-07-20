#pragma once
#include <stdint.h>
#include <SdFat.h>

class SDCard {
public:
    SDCard();
    bool begin();
    bool is_mounted() const;

    bool open_file(const char* path, oflag_t flags);
    void close_file();
    bool read_line(char* buffer, size_t max_len);
    bool write(const uint8_t* data, size_t len);

    uint64_t total_bytes() const;
    uint64_t used_bytes();

private:
    SdFat _sd;
    FsFile _file;
    bool _mounted;
};
