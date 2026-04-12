#include "subsystem_manager.h"
#include "../ble/ble.h"
#include "../ir/ir.h"
#include "../button/button.h"
#include "../rf/rf.h"

SubsystemState SubsystemManager::bleState = SubsystemState::OFF;
IRState SubsystemManager::irState = IRState::OFF;
RFState SubsystemManager::rfState = RFState::OFF;

void SubsystemManager::init() {
    // Button always on
    Button::init();

    // IR starts OFF
    // BLE starts OFF
    // RF starts OFF
}

void SubsystemManager::loop() {
    // Button always runs
    Button::loop();

    // IR runs only when enabled
    if (irState == IRState::ON) {
        IR::loop();
    }

    // BLE runs only when enabled
    if (bleState == SubsystemState::ON) {
        ble_loop();
    }

    // RF runs only when enabled
    if (rfState == RFState::ON) {
        RF::loop();
    }
}

void SubsystemManager::enableBLE() {
    if (bleState == SubsystemState::OFF) {
        ble_init();
        bleState = SubsystemState::ON;
    }
}

void SubsystemManager::disableBLE() {
    if (bleState == SubsystemState::ON) {
        ble_stop();
        bleState = SubsystemState::OFF;
    }
}

bool SubsystemManager::isBLEEnabled() {
    return bleState == SubsystemState::ON;
}

void SubsystemManager::enableIR() {
    if (irState == IRState::OFF) {
        IR::init();
        irState = IRState::ON;
    }
}

void SubsystemManager::disableIR() {
    if (irState == IRState::ON) {
        IR::stop();
        irState = IRState::OFF;
    }
}

bool SubsystemManager::isIREnabled() {
    return irState == IRState::ON;
}

void SubsystemManager::enableRF() {
    if (rfState == RFState::OFF) {
        RF::init();
        rfState = RFState::ON;
    }
}

void SubsystemManager::disableRF() {
    if (rfState == RFState::ON) {
        RF::stop();
        rfState = RFState::OFF;
    }
}

bool SubsystemManager::isRFEnabled() {
    return rfState == RFState::ON;
}