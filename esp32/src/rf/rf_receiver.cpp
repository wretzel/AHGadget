#include "rf_receiver.h"
#include "rf_pins.h"
#include <ELECHOUSE_CC1101_SRC_DRV.h>

namespace RFReceiver {

static bool initialized = false;
static float currentFreq = 315.0;
static unsigned long lastStep = 0;
static unsigned long lastPrint = 0;

void init() {
    if (initialized) return;

    ELECHOUSE_cc1101.setSpiPin(
        CC1101_SCK,
        CC1101_MISO,
        CC1101_MOSI,
        CC1101_CS
    );
    ELECHOUSE_cc1101.setGDO(CC1101_GDO0, CC1101_GDO2);

    ELECHOUSE_cc1101.Init();
    ELECHOUSE_cc1101.setCCMode(1);      // use internal config
    ELECHOUSE_cc1101.setModulation(2);  // ASK/OOK (good for remotes)
    ELECHOUSE_cc1101.setSyncMode(0);    // no sync word
    ELECHOUSE_cc1101.setPktFormat(0);   // raw mode
    ELECHOUSE_cc1101.setMHZ(433.92);    // park here
    ELECHOUSE_cc1101.setRxBW(58);       // reasonable bandwidth
    ELECHOUSE_cc1101.setDRate(4.8);     // low-ish data rate

    ELECHOUSE_cc1101.SetRx();

    initialized = true;
    Serial.println("{\"type\":\"sys\",\"subsystem\":\"rf_recv\",\"state\":\"receiving\"}");
}

void loop() {
    if (!initialized) return;

    // Optional: occasional RSSI debug
    static unsigned long lastRssi = 0;
    unsigned long now = millis();
    if (now - lastRssi > 300) {
        lastRssi = now;
        int rssi = ELECHOUSE_cc1101.getRssi();
        Serial.printf("{\"type\":\"rf\",\"event\":\"rssi\",\"value\":%d}\n", rssi);
    }

    if (ELECHOUSE_cc1101.CheckReceiveFlag()) {
        uint8_t buffer[64];
        uint8_t len = ELECHOUSE_cc1101.ReceiveData(buffer);

        Serial.print("{\"type\":\"rf\",\"event\":\"rx\",\"len\":");
        Serial.print(len);
        Serial.print(",\"data\":\"");

        for (int i = 0; i < len; i++) {
            Serial.printf("%02X", buffer[i]);
        }

        Serial.println("\"}");
    }
}

void stop() {
    if (!initialized) return;
    ELECHOUSE_cc1101.setSidle();
    initialized = false;
}

}
