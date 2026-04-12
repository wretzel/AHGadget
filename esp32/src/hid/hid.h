#pragma once
#include <stdint.h>

namespace hid {

    // Initialize HID subsystem
    void init();

    // Basic keyboard actions
    void keyTap(uint8_t keycode);
    void keyPress(uint8_t keycode);
    void keyRelease(uint8_t keycode);

    // Modifier + key
    void modTap(uint8_t mods, uint8_t keycode);

    // Type full text
    void typeText(const char* text);

    // Delay helper
    void delayMs(uint32_t ms);

}
