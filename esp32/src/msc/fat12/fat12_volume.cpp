#include "fat12_volume.h"
#include <string.h>

namespace fat12 {

bool Volume::load(const uint8_t* s) {
    // Parse BPB fields
    _bpb.bytesPerSector    = *(const uint16_t*)(s + 11);
    _bpb.sectorsPerCluster = *(const uint8_t  *)(s + 13);
    _bpb.reservedSectors   = *(const uint16_t*)(s + 14);
    _bpb.fatCount          = *(const uint8_t  *)(s + 16);
    _bpb.rootEntryCount    = *(const uint16_t*)(s + 17);
    _bpb.totalSectors16    = *(const uint16_t*)(s + 19);
    _bpb.sectorsPerFAT     = *(const uint16_t*)(s + 22);

    // Basic sanity checks to avoid divide‑by‑zero
    if (_bpb.bytesPerSector == 0 ||
        _bpb.sectorsPerCluster == 0 ||
        _bpb.fatCount == 0 ||
        _bpb.sectorsPerFAT == 0) {
        return false;
    }

    // Compute offsets
    _bpb.fatStart     = _bpb.reservedSectors;
    _bpb.rootDirStart = _bpb.fatStart + _bpb.fatCount * _bpb.sectorsPerFAT;

    uint32_t rootDirBytes   = _bpb.rootEntryCount * 32;
    uint32_t rootDirSectors = (rootDirBytes + _bpb.bytesPerSector - 1) / _bpb.bytesPerSector;

    _bpb.dataStart = _bpb.rootDirStart + rootDirSectors;

    return true;
}

uint32_t Volume::fatOffset() const { return _bpb.fatStart; }
uint32_t Volume::rootDirOffset() const { return _bpb.rootDirStart; }
uint32_t Volume::dataOffset() const { return _bpb.dataStart; }

}
