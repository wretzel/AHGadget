#pragma once
#include "rf.h"

namespace RFReceiver {
    void init();
    void enable();
    void disable();
    void loop();
    bool hasFrame();
    bool getFrame(RFFrame &out);
}
