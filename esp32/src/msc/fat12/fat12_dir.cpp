#include "fat12_dir.h"
#include <Arduino.h>
#include <string.h>

namespace fat12 {

// ---------------------------------------------------------
// Helpers
// ---------------------------------------------------------

// 0x00 = end of directory
// 0xE5 = deleted entry
static bool isEntryFree(const DirEntry& e) {
    uint8_t first = *(const uint8_t*)&e;
    return (first == 0x00 || first == 0xE5);
}

static bool isEntryValid(const DirEntry& e) {
    uint8_t first = *(const uint8_t*)&e;
    if (first == 0x00 || first == 0xE5) return false;
    if (e.attr & 0x08) return false; // skip volume labels
    return true;
}

static bool nameMatches(const DirEntry& e, const char* name83) {
    return memcmp(e.name, name83, 11) == 0;
}

// ---------------------------------------------------------
// find()
// ---------------------------------------------------------

bool Directory::find(const char* name83, DirEntry& out) {
    const BPB& bpb = _vol.bpb();
    uint32_t rootOffsetBytes = _vol.rootDirOffset() * bpb.bytesPerSector;
    uint32_t entryCount = bpb.rootEntryCount;

    for (uint32_t i = 0; i < entryCount; i++) {
        DirEntry* e = (DirEntry*)(&_disk[rootOffsetBytes + i * sizeof(DirEntry)]);

        uint8_t first = *(uint8_t*)e;
        if (first == 0x00) break; // end of directory

        if (!isEntryValid(*e)) continue;
        if (nameMatches(*e, name83)) {
            memcpy(&out, e, sizeof(DirEntry));
            return true;
        }
    }

    return false;
}

// ---------------------------------------------------------
// create()
// ---------------------------------------------------------

bool Directory::create(const char* name83, uint16_t firstCluster, uint32_t size) {
    const BPB& bpb = _vol.bpb();
    uint32_t rootOffsetBytes = _vol.rootDirOffset() * bpb.bytesPerSector;
    uint32_t entryCount = bpb.rootEntryCount;

    for (uint32_t i = 0; i < entryCount; i++) {
        DirEntry* e = (DirEntry*)(&_disk[rootOffsetBytes + i * sizeof(DirEntry)]);
        uint8_t first = *(uint8_t*)e;

        if (first == 0x00 || first == 0xE5) {
            // free slot
            memset(e, 0, sizeof(DirEntry));
            memcpy(e->name, name83, 11);

            e->attr = 0x20; // archive
            e->firstClusterLow  = firstCluster;
            e->firstClusterHigh = 0;
            e->fileSize = size;

            return true;
        }
    }

    return false; // no space in root directory
}

// ---------------------------------------------------------
// remove()
// ---------------------------------------------------------

bool Directory::remove(const char* name83) {
    const BPB& bpb = _vol.bpb();
    uint32_t rootOffsetBytes = _vol.rootDirOffset() * bpb.bytesPerSector;
    uint32_t entryCount = bpb.rootEntryCount;

    for (uint32_t i = 0; i < entryCount; i++) {
        DirEntry* e = (DirEntry*)(&_disk[rootOffsetBytes + i * sizeof(DirEntry)]);

        uint8_t first = *(uint8_t*)e;
        if (first == 0x00) break;

        if (!isEntryValid(*e)) continue;
        if (nameMatches(*e, name83)) {
            *(uint8_t*)e = 0xE5; // mark deleted
            return true;
        }
    }

    return false;
}

// ---------------------------------------------------------
// list()
// ---------------------------------------------------------

void Directory::list(Stream& out) {
    const BPB& bpb = _vol.bpb();
    uint32_t rootOffsetBytes = _vol.rootDirOffset() * bpb.bytesPerSector;
    uint32_t entryCount = bpb.rootEntryCount;

    out.print("\"files\":[");

    bool firstOut = true;

    for (uint32_t i = 0; i < entryCount; i++) {
        DirEntry* e = (DirEntry*)(&_disk[rootOffsetBytes + i * sizeof(DirEntry)]);

        uint8_t first = *(uint8_t*)e;
        if (first == 0x00) break;
        if (!isEntryValid(*e)) continue;

        if (!firstOut) out.print(",");
        firstOut = false;

        out.print("{\"name\":\"");

        // Convert 8.3 name to readable form
        // NAME.EXT (trim spaces)
        for (int j = 0; j < 8; j++) {
            if (e->name[j] == ' ') break;
            out.print((char)e->name[j]);
        }

        if (e->name[8] != ' ') {
            out.print(".");
            for (int j = 8; j < 11; j++) {
                if (e->name[j] == ' ') break;
                out.print((char)e->name[j]);
            }
        }

        out.print("\",\"size\":");
        out.print(e->fileSize);
        out.print("}");
    }

    out.print("]");
}

} // namespace fat12
