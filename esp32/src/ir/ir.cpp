#include "ir.h"
#include "ir_receiver.h"
#include "ir_transmitter.h"

namespace IR {

    void init() {
        IRReceiver::init();
        IRTransmitter::init();
    }

    void loop() {
        IRReceiver::loop();
        IRTransmitter::loop();
    }

    void stop() {
        IRReceiver::stop();
        IRTransmitter::stop();
    }

}
