#pragma once

#include <Arduino.h>

struct TelemetryReading {
    float busVoltage;
    float shuntVoltage;
    float current;
    float power;
};

class Telemetry {
public:
    static bool init();
    static TelemetryReading read();
};
