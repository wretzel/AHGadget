#pragma once
#include <stdint.h>
#include "fat12_volume.h"
#include "fat12_fat.h"
#include "fat12_dir.h"

namespace fat12 {

    class File {
    public:
        File(Volume& vol, FAT& fat, Directory& dir, uint8_t* disk)
            : _vol(vol), _fat(fat), _dir(dir), _disk(disk) {}

        bool read(const char* name83, uint8_t* out, uint32_t& size);
        bool write(const char* name83, const uint8_t* data, uint32_t size);

    private:
        Volume&    _vol;
        FAT&       _fat;
        Directory& _dir;
        uint8_t*   _disk;
    };

}
