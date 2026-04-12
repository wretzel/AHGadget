#pragma once
#include <Arduino.h>

namespace RFTransmitter {
    void init();
    void loop();
    void stop();

    void sendRaw(const uint8_t *data, size_t len);
}
