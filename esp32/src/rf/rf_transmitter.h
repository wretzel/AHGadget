#pragma once
#include "rf.h"

namespace RFTransmitter {
    void init();
    void transmit(const RFFrame &frame);
}
