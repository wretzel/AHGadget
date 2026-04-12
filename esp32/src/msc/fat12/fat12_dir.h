#pragma once
#include <Arduino.h> 
#include <stdint.h>
#include "fat12_utils.h"
#include "fat12_volume.h"
#include "fat12_fat.h"

namespace fat12 {

    class Directory {
    public:
        Directory(Volume& vol, FAT& fat, uint8_t* disk)
            : _vol(vol), _fat(fat), _disk(disk) {}

        bool find(const char* name83, DirEntry& out);
        bool create(const char* name83, uint16_t firstCluster, uint32_t size);
        bool remove(const char* name83);

        void list(Stream& out);

    private:
        Volume&  _vol;
        FAT&     _fat;
        uint8_t* _disk;
    };

}
