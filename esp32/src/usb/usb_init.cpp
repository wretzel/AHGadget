#include <Arduino.h>
#include "USB.h"

// Subsystem headers
#include "../msc/msc.h"
#include "../hid/hid.h"

namespace usb {

    void init() {
        // IMPORTANT:
        // We want Serial on UART (COM between Pi & ESP32),
        // NOT on USB CDC. That is controlled via build flags:
        //
        // In platformio.ini:
        //   build_flags =
        //       -DARDUINO_USB_CDC_ON_BOOT=0
        //
        // With CDC off, Serial maps to UART0 and USB is used
        // only for HID + MSC.

        // Initialize HID subsystem (does NOT call USB.begin())
        hid::init();

        // Initialize MSC subsystem (does NOT call USB.begin())
        msc::init();

        // Now start the TinyUSB stack with all registered interfaces
        USB.begin();
    }

    void loop() {
        // For now, nothing needed here.
        // If later you need USB-specific periodic work, put it here.
    }

} // namespace usb
