#pragma once
#include <Arduino.h>

namespace msc {

    // High-level command wrappers that call msc::ops
    bool writeFile(const String& filename, const String& base64Data);
    bool readFile(const String& filename, Stream& out);
    void listFiles(Stream& out);

    bool deleteFile(const String& filename);
    bool formatDisk();
    void info(Stream& out);

}
