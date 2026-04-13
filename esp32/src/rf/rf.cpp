#include "rf.h"
#include "rf_receiver.h"
#include "rf_transmitter.h"

namespace {
    bool enabled = false;
}

namespace RF {

void init() {
    RFReceiver::init();
    RFTransmitter::init();
}

void enable() {
    if (!enabled) {
        RFReceiver::enable();
        enabled = true;
    }
}

void disable() {
    if (enabled) {
        RFReceiver::disable();
        enabled = false;
    }
}

bool isEnabled() {
    return enabled;
}

void loop() {
    if (!enabled) return;

    RFReceiver::loop();

    static RFFrame f;
    if (RFReceiver::hasFrame() && RFReceiver::getFrame(f)) {
        Serial.printf("{\"type\":\"rf\",\"event\":\"rx\",\"count\":%u}\n",
                      (unsigned)f.count);
    }
}

void transmit(const RFFrame &frame) {
    RFTransmitter::transmit(frame);
}

}
