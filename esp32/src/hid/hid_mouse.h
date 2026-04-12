#pragma once
#include <stdint.h>

namespace hid {

    // Initialization (called from hid::init)
    void initMouse();

    // Basic mouse actions
    void mouseMove(int8_t dx, int8_t dy);
    void mouseClick(uint8_t button);
    void mousePress(uint8_t button);
    void mouseRelease(uint8_t button);
    void mouseScroll(int8_t amount);

    // Smooth touchscreen-grade movement
    void mouseSmoothMove(int dx, int dy);

    // Drag gesture (press → smooth move → release)
    void mouseDrag(int dx, int dy);

    // Velocity-scaled movement
    void mouseMoveVelocity(int dx, int dy, float speed);
}
