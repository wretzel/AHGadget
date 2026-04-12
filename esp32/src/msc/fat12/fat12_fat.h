#pragma once
#include <stdint.h>
#include "fat12_utils.h"
#include "fat12_volume.h"

namespace fat12 {

    class FAT {
    public:
        FAT(Volume& vol, uint8_t* disk) : _vol(vol), _disk(disk) {}

        uint16_t readEntry(uint16_t cluster);
        void     writeEntry(uint16_t cluster, uint16_t value);

        uint16_t allocateCluster();
        void     freeChain(uint16_t startCluster);

    private:
        Volume&  _vol;
        uint8_t* _disk;
    };

}
