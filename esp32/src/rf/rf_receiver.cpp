#include "rf_receiver.h"

namespace {
    const int PIN_RX_DAT = 11;
    const uint32_t FRAME_GAP_US = 3000;   // realistic for 433 MHz

    volatile uint32_t lastEdge = 0;
    volatile bool hasEdge = false;
    volatile uint32_t edgeDt = 0;

    RFFrame frame;

    void resetFrame() {
        frame.count = 0;
        frame.valid = false;
    }

    void finalizeFrame() {
        if (frame.count > 0)
            frame.valid = true;
    }

    void IRAM_ATTR rf_isr() {
        uint32_t now = micros();
        edgeDt = now - lastEdge;
        lastEdge = now;

        // Required on ESP32-S3 to keep CHANGE interrupts alive
        volatile uint8_t lvl = digitalRead(PIN_RX_DAT);

        hasEdge = true;
    }
}

namespace RFReceiver {

void init() {
    pinMode(PIN_RX_DAT, INPUT);
    lastEdge = micros();
    resetFrame();
}

void enable() {
    attachInterrupt(digitalPinToInterrupt(PIN_RX_DAT), rf_isr, CHANGE);
}

void disable() {
    detachInterrupt(digitalPinToInterrupt(PIN_RX_DAT));
    resetFrame();
}

void loop() {
    if (!hasEdge) return;

    noInterrupts();
    uint32_t dt = edgeDt;
    hasEdge = false;
    interrupts();

    // --- Frame boundary ---
    if (dt > FRAME_GAP_US) {
        finalizeFrame();
        return;   // DO NOT reset here
    }

    // --- Overflow ---
    if (frame.count >= RFFrame::MAX_EDGES) {
        finalizeFrame();
        return;   // DO NOT reset here
    }

    // --- Store pulse/gap ---
    if (frame.count % 2 == 0)
        frame.pulses[frame.count] = dt;
    else
        frame.gaps[frame.count] = dt;

    frame.count++;
}

bool hasFrame() {
    return frame.valid;
}

bool getFrame(RFFrame &out) {
    if (!frame.valid) return false;
    out = frame;
    resetFrame();   // <-- Only reset AFTER Pi consumes it
    return true;
}

}
