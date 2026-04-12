#include "rf.h"
#include "rf_receiver.h"
#include "rf_transmitter.h"

namespace RF {

void init() {
    RFReceiver::init();
    RFTransmitter::init();
}

void loop() {
    RFReceiver::loop();
    RFTransmitter::loop();
}

void stop() {
    RFReceiver::stop();
    RFTransmitter::stop();
}

}
