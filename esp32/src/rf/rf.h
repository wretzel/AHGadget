#pragma once
#include <Arduino.h>

struct RFFrame {
    static const size_t MAX_EDGES = 8192;
    uint16_t pulses[MAX_EDGES];
    uint16_t gaps[MAX_EDGES];
    size_t count;
    bool valid;
};

namespace RF {
    void init();
    void enable();
    void disable();
    bool isEnabled();
    void loop();
    void transmit(const RFFrame &frame);
}
