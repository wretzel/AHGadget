#include "telemetry.h"
#include "ina219.h"

bool Telemetry::init() {
    return INA219Driver::init();
}

TelemetryReading Telemetry::read() {
    TelemetryReading r;

    r.busVoltage   = INA219Driver::getBusVoltage();
    r.shuntVoltage = INA219Driver::getShuntVoltage();
    r.current      = INA219Driver::getCurrent();
    r.power        = INA219Driver::getPower();

    return r;
}
