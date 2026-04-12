#include "system.h"
#include "command_dispatch.h"
#include "subsystem_manager.h"
#include "../usb/usb_init.h"
#include <Arduino.h>

static String input;

namespace System {

void init() {
    Serial.begin(115200);
    delay(300);

    SubsystemManager::init();
    usb::init();
}

void loop() {
    SubsystemManager::loop();

    while (Serial.available()) {
        char c = Serial.read();

        if (c == '\n' || c == '\r') {
            if (input.length() > 0) {
                CommandDispatch::handleCommand(input);
                input = "";
            }
        } else {
            input += c;
        }
    }
}

}
