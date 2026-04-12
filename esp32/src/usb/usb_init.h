#pragma once

namespace usb {

    // Initialize all USB subsystems (HID, MSC, etc.)
    void init();

    // Optional: call periodically if any USB tasks are needed later
    void loop();

} // namespace usb
