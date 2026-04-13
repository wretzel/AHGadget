#include "rf_transmitter.h"

namespace {
    const int PIN_TX_DAT = 6;
    const int PIN_TX_EN  = 7;
}

namespace RFTransmitter {

void init() {
    pinMode(PIN_TX_DAT, OUTPUT);
    pinMode(PIN_TX_EN, OUTPUT);
    digitalWrite(PIN_TX_DAT, LOW);
    digitalWrite(PIN_TX_EN, LOW);
}

void transmit(const RFFrame &frame) {
    if (!frame.valid || frame.count == 0) return;

    digitalWrite(PIN_TX_EN, HIGH);

    uint8_t level = LOW;
    for (size_t i = 0; i < frame.count; i++) {
        level = !level;
        digitalWrite(PIN_TX_DAT, level);

        uint16_t dur = (i % 2 == 0)
            ? frame.pulses[i]
            : frame.gaps[i];

        delayMicroseconds(dur);
    }

    digitalWrite(PIN_TX_DAT, LOW);
    digitalWrite(PIN_TX_EN, LOW);
}

}
