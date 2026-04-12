#include "fat12_file.h"
#include <string.h>

namespace fat12 {

// ---------------------------------------------------------
// Walk a cluster chain and copy data out
// ---------------------------------------------------------
bool File::read(const char* name83, uint8_t* out, uint32_t& size) {
    DirEntry entry;
    if (!_dir.find(name83, entry)) {
        return false;
    }

    size = entry.fileSize;
    if (size == 0) return true;

    uint16_t cluster = entry.firstClusterLow;
    uint32_t bytesPerCluster = _vol.bpb().bytesPerSector * _vol.bpb().sectorsPerCluster;
    uint32_t dataStart = _vol.dataOffset() * _vol.bpb().bytesPerSector;

    uint32_t remaining = size;
    uint32_t outPos = 0;

    while (cluster >= 2 && cluster < 0xFF0) {
        uint32_t clusterOffset = dataStart + (cluster - 2) * bytesPerCluster;
        uint32_t chunk = (remaining < bytesPerCluster) ? remaining : bytesPerCluster;

        memcpy(out + outPos, &_disk[clusterOffset], chunk);

        outPos += chunk;
        remaining -= chunk;

        if (remaining == 0) break;

        cluster = _fat.readEntry(cluster);
        if (cluster >= 0xFF0) break;
    }

    return true;
}

// ---------------------------------------------------------
// Write a file: allocate clusters, write data, update directory
// ---------------------------------------------------------
bool File::write(const char* name83, const uint8_t* data, uint32_t size) {
    // If file exists, delete old cluster chain
    DirEntry oldEntry;
    if (_dir.find(name83, oldEntry)) {
        uint16_t oldCluster = oldEntry.firstClusterLow;
        if (oldCluster >= 2) {
            _fat.freeChain(oldCluster);
        }
        _dir.remove(name83);
    }

    // Special case: empty file
    if (size == 0) {
        return _dir.create(name83, 0, 0);
    }

    uint32_t bytesPerCluster = _vol.bpb().bytesPerSector * _vol.bpb().sectorsPerCluster;
    uint32_t dataStart = _vol.dataOffset() * _vol.bpb().bytesPerSector;

    uint32_t remaining = size;
    uint32_t pos = 0;

    uint16_t firstCluster = 0;
    uint16_t prevCluster = 0;

    // Allocate clusters and write data
    while (remaining > 0) {
        uint16_t cluster = _fat.allocateCluster();
        if (cluster == 0) return false; // no space

        if (firstCluster == 0) firstCluster = cluster;
        if (prevCluster != 0) _fat.writeEntry(prevCluster, cluster);

        uint32_t clusterOffset = dataStart + (cluster - 2) * bytesPerCluster;
        uint32_t chunk = (remaining < bytesPerCluster) ? remaining : bytesPerCluster;

        memcpy(&_disk[clusterOffset], data + pos, chunk);

        pos += chunk;
        remaining -= chunk;
        prevCluster = cluster;
    }

    // Mark last cluster as EOC
    _fat.writeEntry(prevCluster, FAT12_EOC);

    // Create directory entry
    return _dir.create(name83, firstCluster, size);
}

} // namespace fat12
