#include "fat12_fat.h"
#include <string.h>

namespace fat12 {

static inline uint32_t fatOffsetBytes(const Volume& vol) {
    return vol.fatOffset() * vol.bpb().bytesPerSector;
}

uint16_t FAT::readEntry(uint16_t cluster) {
    uint32_t base = fatOffsetBytes(_vol);
    uint32_t offset = (cluster * 3) / 2;

    const uint8_t* fat = _disk + base;

    uint16_t value;

    if (cluster & 1) {
        // Odd cluster
        value = ((fat[offset] >> 4) | (fat[offset + 1] << 4)) & 0x0FFF;
    } else {
        // Even cluster
        value = (fat[offset] | ((fat[offset + 1] & 0x0F) << 8)) & 0x0FFF;
    }

    return value;
}

void FAT::writeEntry(uint16_t cluster, uint16_t value) {
    value &= 0x0FFF;

    uint32_t base = fatOffsetBytes(_vol);
    uint32_t offset = (cluster * 3) / 2;

    uint8_t* fat = _disk + base;

    if (cluster & 1) {
        // Odd cluster
        fat[offset]     = (fat[offset] & 0x0F) | ((value << 4) & 0xF0);
        fat[offset + 1] = (value >> 4) & 0xFF;
    } else {
        // Even cluster
        fat[offset]     = value & 0xFF;
        fat[offset + 1] = (fat[offset + 1] & 0xF0) | ((value >> 8) & 0x0F);
    }

    // Mirror FAT1 → FAT2
    uint32_t fatSize = _vol.bpb().sectorsPerFAT * _vol.bpb().bytesPerSector;
    memcpy(fat + fatSize, fat, fatSize);
}

uint16_t FAT::allocateCluster() {
    for (uint16_t c = 2; c < 4085; c++) {
        if (readEntry(c) == FAT12_FREE) {
            writeEntry(c, FAT12_EOC);
            return c;
        }
    }
    return 0;
}

void FAT::freeChain(uint16_t start) {
    uint16_t c = start;
    while (c >= 2 && c < 4085) {
        uint16_t next = readEntry(c);
        writeEntry(c, FAT12_FREE);
        if (next >= FAT12_EOC) break;
        c = next;
    }
}

}
