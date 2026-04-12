#pragma once
#include <Arduino.h>

enum class SubsystemState {
    OFF,
    ON
};

enum class IRState {
    OFF,
    ON
};

enum class RFState {
    OFF,
    ON
};

class SubsystemManager {
public:
    static void init();
    static void loop();

    static void enableBLE();
    static void disableBLE();
    static bool isBLEEnabled();

    static void enableIR();
    static void disableIR();
    static bool isIREnabled();

    static void enableRF();
    static void disableRF();
    static bool isRFEnabled();

private:
    static SubsystemState bleState;
    static IRState irState;
    static RFState rfState;
};
