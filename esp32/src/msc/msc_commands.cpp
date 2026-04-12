#include "msc_commands.h"
#include "msc_ops.h"

namespace msc {

bool writeFile(const String& filename, const String& base64Data) {
    return ops::writeFile(filename, base64Data);
}

bool readFile(const String& filename, Stream& out) {
    return ops::readFile(filename, out);
}

void listFiles(Stream& out) {
    ops::listFiles(out);
}

bool deleteFile(const String& filename) {
    return ops::deleteFile(filename);
}

bool formatDisk() {
    return ops::formatDisk();
}

void info(Stream& out) {
    ops::info(out);
}

} // namespace msc
