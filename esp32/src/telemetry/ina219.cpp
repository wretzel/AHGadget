#include "ina219.h"
#include <Adafruit_INA219.h>

static Adafruit_INA219 ina219;

bool INA219Driver::init() {
    return ina219.begin();
}

float INA219Driver::getBusVoltage() {
    return ina219.getBusVoltage_V();
}

float INA219Driver::getShuntVoltage() {
    return ina219.getShuntVoltage_mV();
}

float INA219Driver::getCurrent() {
    return ina219.getCurrent_mA();
}

float INA219Driver::getPower() {
    return ina219.getPower_mW();
}
