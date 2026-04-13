#pragma once

class INA219Driver {
public:
    static bool init();

    static float getBusVoltage();
    static float getShuntVoltage();
    static float getCurrent();
    static float getPower();
};
