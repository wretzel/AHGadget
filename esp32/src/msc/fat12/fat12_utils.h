#pragma once
#include <stdint.h>

namespace fat12 {

    // BIOS Parameter Block (boot sector)
    struct BPB {
        uint16_t bytesPerSector;
        uint8_t  sectorsPerCluster;
        uint16_t reservedSectors;
        uint8_t  fatCount;
        uint16_t rootEntryCount;
        uint16_t totalSectors16;
        uint16_t sectorsPerFAT;
        uint32_t fatStart;
        uint32_t rootDirStart;
        uint32_t dataStart;
    };

    // Directory entry (8.3 format)
    struct DirEntry {
        char     name[11];
        uint8_t  attr;
        uint8_t  reserved;
        uint8_t  createTimeFine;
        uint16_t createTime;
        uint16_t createDate;
        uint16_t accessDate;
        uint16_t firstClusterHigh;
        uint16_t writeTime;
        uint16_t writeDate;
        uint16_t firstClusterLow;
        uint32_t fileSize;
    };

    // FAT12 constants
    static const uint16_t FAT12_FREE = 0x000;
    static const uint16_t FAT12_EOC  = 0xFFF;

}
