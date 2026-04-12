#pragma once
#include <Arduino.h>

namespace IRTransmitter {

    void init();
    void loop();
    void stop();

    // Called by CommandDispatcher when Pi sends a raw IR frame
    void loadFrame(const uint16_t *pulses, const uint16_t *gaps, size_t count);
    void testFlash();

}
