#pragma once
#include <Arduino.h>

// Bring in FAT12 types
#include "fat12/fat12_volume.h"
#include "fat12/fat12_fat.h"
#include "fat12/fat12_dir.h"
#include "fat12/fat12_file.h"

namespace msc {

    // RAM disk buffer (defined in msc.cpp)
    extern uint8_t ramdisk[];

    // MSC subsystem
    void init();
    void loop();

    // FAT12 accessors
    fat12::Directory* getDirectory();
    fat12::File*      getFile();
    fat12::Volume*    getVolume();
    fat12::FAT*       getFAT();

}
