#include "hid.h"
#include <Arduino.h>
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBHIDMouse.h"
#include "hid_mouse.h"

// Global keyboard instance
static USBHIDKeyboard Keyboard;

namespace hid {

void init() {
    USB.begin();          // Start TinyUSB first
    Keyboard.begin();     // Then register keyboard
    hid::initMouse();     // Then register mouse
}

// ---------------------------------------------------------
// Basic key actions
// ---------------------------------------------------------

void keyPress(uint8_t keycode) {
    Keyboard.press(keycode);
}

void keyRelease(uint8_t keycode) {
    Keyboard.release(keycode);
}

void keyTap(uint8_t keycode) {
    Keyboard.press(keycode);
    delay(5);
    Keyboard.release(keycode);
}

// ---------------------------------------------------------
// Modifier + key
// ---------------------------------------------------------

void modTap(uint8_t mods, uint8_t keycode) {
    Keyboard.press(mods);
    Keyboard.press(keycode);
    delay(5);
    Keyboard.release(keycode);
    Keyboard.release(mods);
}

// ---------------------------------------------------------
// Type full text
// ---------------------------------------------------------

void typeText(const char* text) {
    Keyboard.print(text);
}

// ---------------------------------------------------------
// Delay helper
// ---------------------------------------------------------

void delayMs(uint32_t ms) {
    delay(ms);
}

} // namespace hid
