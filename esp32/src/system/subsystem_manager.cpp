#include "subsystem_manager.h"
#include "../ble/ble.h"
#include "../ir/ir.h"
#include "../button/button.h"
#include "../rf/rf.h"
#include "../telemetry/telemetry.h"

SubsystemState SubsystemManager::bleState = SubsystemState::OFF;
IRState SubsystemManager::irState = IRState::OFF;
RFState SubsystemManager::rfState = RFState::OFF;

void SubsystemManager::init() {
    // Button always on
    Button::init();

    // Telemetry always on
    Telemetry::init();

    // RF subsystem initializes once at boot
    RF::init();
}

void SubsystemManager::loop() {
    Button::loop();

    // Telemetry push mode (1 Hz)
    static unsigned long lastTelemetry = 0;
    if (millis() - lastTelemetry >= 1000) {
        lastTelemetry = millis();
        TelemetryReading r = Telemetry::read();
        Serial.printf(
            "{\"type\":\"telemetry\",\"bus\":%.3f,\"shunt\":%.3f,\"current\":%.3f,\"power\":%.3f}\n",
            r.busVoltage, r.shuntVoltage, r.current, r.power
        );
    }

    // Heartbeat (1 Hz)
    static unsigned long lastHeartbeat = 0;
    if (millis() - lastHeartbeat >= 1000) {
        lastHeartbeat = millis();
        Serial.println("{\"type\":\"sys\",\"event\":\"heartbeat\"}");
    }

    if (irState == IRState::ON) {
        IR::loop();
    }

    if (bleState == SubsystemState::ON) {
        ble_loop();
    }

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
        RF::enable();
        rfState = RFState::ON;
    }
}

void SubsystemManager::disableRF() {
    if (rfState == RFState::ON) {
        RF::disable();
        rfState = RFState::OFF;
    }
}

bool SubsystemManager::isRFEnabled() {
    return rfState == RFState::ON;
}
