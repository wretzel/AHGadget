#include "hid_mouse.h"
#include <Arduino.h>
#include "USB.h"
#include "USBHIDMouse.h"

static USBHIDMouse Mouse;

namespace hid {

// ---------------------------------------------------------
// Initialization (called from hid::init())
// ---------------------------------------------------------
void initMouse() {
    Mouse.begin();
}

// ---------------------------------------------------------
// Basic mouse movement
// ---------------------------------------------------------
void mouseMove(int8_t dx, int8_t dy) {
    Mouse.move(dx, dy);
}

// ---------------------------------------------------------
// Click (press + release)
// ---------------------------------------------------------
void mouseClick(uint8_t button) {
    Mouse.click(button);
}

// ---------------------------------------------------------
// Press and hold
// ---------------------------------------------------------
void mousePress(uint8_t button) {
    Mouse.press(button);
}

// ---------------------------------------------------------
// Release button
// ---------------------------------------------------------
void mouseRelease(uint8_t button) {
    Mouse.release(button);
}

// ---------------------------------------------------------
// Scroll wheel (TinyUSB uses wheel in 3rd param)
// ---------------------------------------------------------
void mouseScroll(int8_t amount) {
    Mouse.move(0, 0, amount);
}

// ---------------------------------------------------------
// Smooth interpolated movement (touchscreen-grade)
// ---------------------------------------------------------
void mouseSmoothMove(int dx, int dy) {
    // Deadzone to remove touchscreen jitter
    if (abs(dx) < 2 && abs(dy) < 2) {
        return;
    }

    int steps = max(abs(dx), abs(dy));
    if (steps == 0) return;

    float sx = dx / (float)steps;
    float sy = dy / (float)steps;

    for (int i = 0; i < steps; i++) {
        Mouse.move((int8_t)sx, (int8_t)sy);
        delayMicroseconds(500); // ~2 kHz smooth motion
    }
}

// ---------------------------------------------------------
// Drag gesture (press → smooth move → release)
// ---------------------------------------------------------
void mouseDrag(int dx, int dy) {
    Mouse.press(MOUSE_BUTTON_LEFT);
    mouseSmoothMove(dx, dy);
    Mouse.release(MOUSE_BUTTON_LEFT);
}

// ---------------------------------------------------------
// Velocity-scaled movement
// ---------------------------------------------------------
void mouseMoveVelocity(int dx, int dy, float speed) {
    int vx = dx * speed;
    int vy = dy * speed;
    mouseSmoothMove(vx, vy);
}

} // namespace hid
