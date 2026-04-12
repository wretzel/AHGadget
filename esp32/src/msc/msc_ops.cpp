#include <Arduino.h>
#include <memory>
#include "msc_ops.h"
#include "msc.h"
#include "fat12/fat12_utils.h"


// Base64 helpers
extern "C" {
    int base64_dec_len(const char* input, int length);
    int base64_decode(char* output, const char* input, int length);
    int base64_enc_len(int inputLen);
    int base64_encode(char* output, const char* input, int inputLen);
}

using msc::ramdisk;

namespace msc::ops {

// ---------------------------------------------------------
// Convert "TEST.TXT" → "TEST    TXT"
// ---------------------------------------------------------
bool to83(const String& name, char out[11]) {
    String upper = name;
    upper.toUpperCase();

    int dot = upper.indexOf('.');
    String base = (dot == -1) ? upper : upper.substring(0, dot);
    String ext  = (dot == -1) ? ""    : upper.substring(dot + 1);

    if (base.length() > 8 || ext.length() > 3) return false;

    memset(out, ' ', 11);
    memcpy(out, base.c_str(), base.length());
    memcpy(out + 8, ext.c_str(), ext.length());
    return true;
}

// ---------------------------------------------------------
// Write file
// ---------------------------------------------------------
bool writeFile(const String& filename, const String& base64Data) {
    char name83[11];
    if (!to83(filename, name83)) return false;

    int decodedLen = base64_dec_len(base64Data.c_str(), base64Data.length());
    std::unique_ptr<uint8_t[]> decoded(new uint8_t[decodedLen]);
    base64_decode((char*)decoded.get(), base64Data.c_str(), base64Data.length());

    auto file = msc::getFile();
    if (!file) return false;

    return file->write(name83, decoded.get(), decodedLen);
}

// ---------------------------------------------------------
// Read file
// ---------------------------------------------------------
bool readFile(const String& filename, Stream& out) {
    char name83[11];
    if (!to83(filename, name83)) return false;

    auto file = msc::getFile();
    if (!file) return false;

    uint32_t size = 0;
    std::unique_ptr<uint8_t[]> buf(new uint8_t[65536]);

    if (!file->read(name83, buf.get(), size)) return false;

    int encodedLen = base64_enc_len(size);
    std::unique_ptr<char[]> encoded(new char[encodedLen + 1]);
    base64_encode(encoded.get(), (char*)buf.get(), size);

    out.print(encoded.get());
    return true;
}

// ---------------------------------------------------------
// List files
// ---------------------------------------------------------
void listFiles(Stream& out) {
    auto dir = msc::getDirectory();
    if (!dir) {
        out.print("\"files\":[]");
        return;
    }
    dir->list(out);
}

// ---------------------------------------------------------
// Delete file
// ---------------------------------------------------------
bool deleteFile(const String& filename) {
    char name83[11];
    if (!to83(filename, name83)) return false;

    auto dir = msc::getDirectory();
    auto fat = msc::getFAT();
    if (!dir || !fat) return false;

    fat12::DirEntry entry;
    if (!dir->find(name83, entry)) return false;

    uint16_t first = entry.firstClusterLow;
    if (first >= 2) {
        fat->freeChain(first);
    }

    return dir->remove(name83);
}

// ---------------------------------------------------------
// Format disk
// ---------------------------------------------------------
bool formatDisk() {
    // 64 KB disk: 128 sectors * 512 bytes
    memset(ramdisk, 0, 512 * 128);

    uint8_t* b = ramdisk;

    // Jump + OEM
    b[0] = 0xEB; b[1] = 0x3C; b[2] = 0x90;
    memcpy(b + 3, "MSDOS5.0", 8);

    // BPB
    *(uint16_t*)(b + 11) = 512;   // bytes per sector
    b[13] = 1;                    // sectors per cluster
    *(uint16_t*)(b + 14) = 1;     // reserved sectors
    b[16] = 2;                    // FAT count
    *(uint16_t*)(b + 17) = 224;   // root entries
    *(uint16_t*)(b + 19) = 128;   // total sectors
    b[21] = 0xF8;                 // media
    *(uint16_t*)(b + 22) = 1;     // sectors per FAT
    *(uint16_t*)(b + 24) = 1;     // sectors per track
    *(uint16_t*)(b + 26) = 1;     // heads
    *(uint32_t*)(b + 28) = 0;     // hidden sectors
    *(uint32_t*)(b + 32) = 0;     // total sectors (32-bit, unused)

    b[510] = 0x55;
    b[511] = 0xAA;

    // ---- Initialize FAT1 and FAT2 ----
    const uint16_t bytesPerSector   = 512;
    const uint8_t  fatCount         = 2;
    const uint16_t sectorsPerFAT    = 1;
    const uint16_t reservedSectors  = 1;

    uint32_t fatStartSector = reservedSectors;
    uint32_t fatSizeBytes   = sectorsPerFAT * bytesPerSector;

    // FAT1 base
    uint8_t* fat1 = ramdisk + fatStartSector * bytesPerSector;
    // FAT2 base
    uint8_t* fat2 = fat1 + fatSizeBytes;

    // FAT12 reserved entries:
    // cluster 0: media + reserved
    // cluster 1: reserved (EOC)
    // Encoded as: [media][FF][FF]
    fat1[0] = 0xF8;  // media
    fat1[1] = 0xFF;
    fat1[2] = 0xFF;

    // Mirror to FAT2
    memcpy(fat2, fat1, fatSizeBytes);

    // Root dir + data area are already zeroed by memset

    return true;
}

// ---------------------------------------------------------
// Info
// ---------------------------------------------------------
void info(Stream& out) {
    auto vol = msc::getVolume();
    if (!vol) {
        out.print("\"error\":\"no volume\"");
        return;
    }

    const auto& bpb = vol->bpb();

    out.print("\"bpb\":{");
    out.print("\"bytesPerSector\":"); out.print(bpb.bytesPerSector); out.print(",");
    out.print("\"sectorsPerCluster\":"); out.print(bpb.sectorsPerCluster); out.print(",");
    out.print("\"reservedSectors\":"); out.print(bpb.reservedSectors); out.print(",");
    out.print("\"fatCount\":"); out.print(bpb.fatCount); out.print(",");
    out.print("\"rootEntryCount\":"); out.print(bpb.rootEntryCount); out.print(",");
    out.print("\"totalSectors16\":"); out.print(bpb.totalSectors16); out.print(",");
    out.print("\"sectorsPerFAT\":"); out.print(bpb.sectorsPerFAT);
    out.print("}");
}

} // namespace msc::ops
