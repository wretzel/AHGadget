#include "rf_transmitter.h"
#include "rf_pins.h"
#include <ELECHOUSE_CC1101_SRC_DRV.h>

namespace RFTransmitter {

static bool initialized = false;

void init() {
    if (initialized) {
        Serial.println("{\"type\":\"sys\",\"subsystem\":\"rf_tx\",\"state\":\"off\"}");
        return;
    }

    // Configure pins BEFORE Init()
    ELECHOUSE_cc1101.setSpiPin(
        CC1101_SCK,
        CC1101_MISO,
        CC1101_MOSI,
        CC1101_CS
    );

    ELECHOUSE_cc1101.setGDO(CC1101_GDO0, CC1101_GDO2);

    // TX uses same CC1101 config as RX
    initialized = true;

    Serial.println("{\"type\":\"sys\",\"subsystem\":\"rf_tx\",\"state\":\"idle\"}");
}

void loop() {
    // No continuous TX loop needed
}

void stop() {
    if (!initialized) return;

    ELECHOUSE_cc1101.setSidle();
    initialized = false;

    Serial.println("{\"type\":\"sys\",\"subsystem\":\"rf_tx\",\"state\":\"off\"}");
}

void sendRaw(const uint8_t *data, size_t len) {
    if (!initialized) return;

    ELECHOUSE_cc1101.SetTx();
    ELECHOUSE_cc1101.SendData((byte*)data, (byte)len);
    ELECHOUSE_cc1101.SetRx();

    Serial.printf("{\"type\":\"rf\",\"event\":\"tx\",\"len\":%u}\n", (unsigned)len);
}

}
