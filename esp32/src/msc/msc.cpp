#include <Arduino.h>
#include "USBMSC.h"
#include "SPIFFS.h"
#include "msc.h"

// FAT12 includes (full definitions)
#include "fat12/fat12_volume.h"
#include "fat12/fat12_fat.h"
#include "fat12/fat12_dir.h"
#include "fat12/fat12_file.h"

// MSC ops (for formatDisk)
#include "msc_ops.h"

namespace msc {

    // -----------------------------
    // MSC block device parameters
    // -----------------------------
    static const uint32_t BLOCK_SIZE  = 512;
    static const uint32_t BLOCK_COUNT = 128;   // 64 KB
    static const size_t   DISK_SIZE   = BLOCK_SIZE * BLOCK_COUNT;

    // Exported RAM disk buffer
    uint8_t ramdisk[DISK_SIZE];

    static const char* MSC_IMAGE_PATH = "/msc.img";

    // USB MSC instance (renamed to avoid namespace conflict)
    static USBMSC usbmsc;

    // -----------------------------
    // FAT12 objects
    // -----------------------------
    static fat12::Volume*     g_volume = nullptr;
    static fat12::FAT*        g_fat    = nullptr;
    static fat12::Directory*  g_dir    = nullptr;
    static fat12::File*       g_file   = nullptr;

    // -----------------------------
    // Flash persistence helpers
    // -----------------------------
    static void saveToFlash();   // forward declaration

    static void loadFromFlash() {
        if (!SPIFFS.begin(true)) {
            // No SPIFFS → create fresh FAT12 volume
            memset(ramdisk, 0, sizeof(ramdisk));
            msc::ops::formatDisk();
            return;
        }

        if (!SPIFFS.exists(MSC_IMAGE_PATH)) {
            // No image → create fresh FAT12 volume
            memset(ramdisk, 0, sizeof(ramdisk));
            msc::ops::formatDisk();
            saveToFlash();
            return;
        }

        File f = SPIFFS.open(MSC_IMAGE_PATH, "r");
        if (!f || f.size() != (int)DISK_SIZE) {
            // Corrupt or wrong size → reset and format
            memset(ramdisk, 0, sizeof(ramdisk));
            if (f) f.close();
            msc::ops::formatDisk();
            saveToFlash();
            return;
        }

        f.read(ramdisk, DISK_SIZE);
        f.close();
    }

    static void saveToFlash() {
        File f = SPIFFS.open(MSC_IMAGE_PATH, "w");
        if (!f) return;
        f.write(ramdisk, DISK_SIZE);
        f.close();
    }

    // -----------------------------
    // TinyUSB MSC Callbacks
    // -----------------------------
    int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) {
        memcpy(buffer, ramdisk + lba * BLOCK_SIZE + offset, bufsize);
        return bufsize;
    }

    int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) {
        memcpy(ramdisk + lba * BLOCK_SIZE + offset, buffer, bufsize);
        saveToFlash();   // write-through persistence
        return bufsize;
    }

    bool onStartStop(uint8_t power_condition, bool start, bool load_eject) {
        if (!start && load_eject) {
            saveToFlash();
        }
        return true;
    }

    // -----------------------------
    // Public API
    // -----------------------------
    void init() {
        // Load or create disk image
        loadFromFlash();

        // Initialize FAT12 stack
        g_volume = new fat12::Volume();

        if (!g_volume->load(ramdisk)) {
            // BPB invalid → format and try again
            msc::ops::formatDisk();
            g_volume->load(ramdisk);
        }

        g_fat  = new fat12::FAT(*g_volume, ramdisk);
        g_dir  = new fat12::Directory(*g_volume, *g_fat, ramdisk);
        g_file = new fat12::File(*g_volume, *g_fat, *g_dir, ramdisk);

        // Configure USB MSC
        usbmsc.vendorID("ESP32");
        usbmsc.productID("AHGadget MSC");
        usbmsc.productRevision("1.0");

        usbmsc.onRead(onRead);
        usbmsc.onWrite(onWrite);
        usbmsc.onStartStop(onStartStop);

        usbmsc.mediaPresent(true);
        usbmsc.begin(BLOCK_COUNT, BLOCK_SIZE);
    }


    void loop() {
        // Reserved for future enhancements
    }

    // -----------------------------
    // FAT12 accessors
    // -----------------------------
    fat12::Directory* getDirectory() { return g_dir; }
    fat12::File*      getFile()      { return g_file; }
    fat12::Volume*    getVolume()    { return g_volume; }
    fat12::FAT*       getFAT()       { return g_fat; }

} // namespace msc
