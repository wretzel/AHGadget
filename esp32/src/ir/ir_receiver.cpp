#include "ir_receiver.h"

// Hardware pin for TSOP38238
static const int IR_RECV_PIN = 5;

// Receiver state
enum class RecvState {
    OFF,
    INITIALIZED,
    RECEIVING
};

static RecvState state = RecvState::OFF;

// Timing capture
static int lastLevel = HIGH;
static unsigned long lastChange = 0;

// Frame boundary threshold (AC remotes often use 20–40 ms gaps)
static const unsigned long FRAME_GAP_US = 20000;

namespace IRReceiver {

void init() {
    if (state != RecvState::OFF) {
        Serial.println("{\"type\":\"sys\",\"subsystem\":\"ir_recv\",\"state\":\"off\"}");
        return;
    }

    pinMode(IR_RECV_PIN, INPUT);

    lastLevel = digitalRead(IR_RECV_PIN);
    lastChange = micros();

    state = RecvState::INITIALIZED;

    Serial.println("{\"type\":\"sys\",\"subsystem\":\"ir_recv\",\"state\":\"off\"}");
}

void loop() {
    if (state == RecvState::INITIALIZED) {
        state = RecvState::RECEIVING;

        Serial.println("{\"type\":\"ir\",\"event\":\"recv_start\",\"ts\":" + String(millis()) + "}");
        Serial.println("{\"type\":\"sys\",\"subsystem\":\"ir_recv\",\"state\":\"receiving\"}");
    }

    if (state != RecvState::RECEIVING) return;

    int level = digitalRead(IR_RECV_PIN);

    // Detect level change
    if (level != lastLevel) {
        unsigned long now = micros();
        unsigned long dur = now - lastChange;

        // Emit pulse/gap event
        Serial.print("{\"type\":\"ir\",\"event\":\"pulse\",\"level\":\"");
        Serial.print(lastLevel == LOW ? "low" : "high");
        Serial.print("\",\"dur\":");
        Serial.print(dur);
        Serial.println("}");

        // Frame boundary detection
        if (lastLevel == HIGH && dur > FRAME_GAP_US) {
            Serial.println("{\"type\":\"ir\",\"event\":\"frame_end\"}");
        }

        lastLevel = level;
        lastChange = now;
    }
}

void stop() {
    if (state == RecvState::RECEIVING) {
        Serial.println("{\"type\":\"ir\",\"event\":\"recv_stop\",\"ts\":" + String(millis()) + "}");
    }

    state = RecvState::OFF;

    Serial.println("{\"type\":\"sys\",\"subsystem\":\"ir_recv\",\"state\":\"off\"}");
}

} // namespace IRReceiver
