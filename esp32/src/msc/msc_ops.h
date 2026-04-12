#pragma once
#include <Arduino.h>

namespace msc::ops {

    // Convert filename to 8.3 format
    bool to83(const String& name, char out[11]);

    // Write a file (base64 → bytes → FAT12)
    bool writeFile(const String& filename, const String& base64Data);

    // Read a file (FAT12 → bytes → base64)
    bool readFile(const String& filename, Stream& out);

    // List files in FAT12 root directory
    void listFiles(Stream& out);

    // Delete a file
    bool deleteFile(const String& filename);

    // Format the FAT12 filesystem
    bool formatDisk();

    // Print FAT12 metadata
    void info(Stream& out);

}
