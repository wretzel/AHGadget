#include "ir_transmitter.h"

// Hardware pin for IR LED
static const int IR_LED_PIN = 4;

// 38 kHz PWM parameters
static const int PWM_CHANNEL = 0;
static const int PWM_FREQ = 38000;
static const int PWM_RES = 8; // 8-bit resolution

// Transmitter state
enum class TxState {
    OFF,
    INITIALIZED,
    IDLE,
    SENDING
};

static TxState state = TxState::OFF;

// Raw frame buffers (max ~300 entries for AC remotes)
static const size_t MAX_ENTRIES = 400;
static uint16_t pulseBuf[MAX_ENTRIES];
static uint16_t gapBuf[MAX_ENTRIES];
static size_t frameCount = 0;

// Playback cursor
static size_t cursor = 0;
static unsigned long segmentStart = 0;
static bool sendingPulse = false;

namespace IRTransmitter {

void init() {
    if (state != TxState::OFF) {
        Serial.println("{\"type\":\"sys\",\"subsystem\":\"ir_tx\",\"state\":\"off\"}");
        return;
    }

    // Setup PWM for 38 kHz carrier
    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES);
    ledcAttachPin(IR_LED_PIN, PWM_CHANNEL);

    // Start with carrier OFF
    ledcWrite(PWM_CHANNEL, 0);

    state = TxState::INITIALIZED;

    Serial.println("{\"type\":\"sys\",\"subsystem\":\"ir_tx\",\"state\":\"off\"}");
}

void loadFrame(const uint16_t *pulses, const uint16_t *gaps, size_t count) {
    if (count > MAX_ENTRIES) count = MAX_ENTRIES;

    for (size_t i = 0; i < count; i++) {
        pulseBuf[i] = pulses[i];
        gapBuf[i] = gaps[i];
    }

    frameCount = count;
    cursor = 0;
    sendingPulse = true;
    segmentStart = micros();

    state = TxState::SENDING;

    Serial.println("{\"type\":\"ir\",\"event\":\"tx_start\",\"count\":" + String(count) + "}");
}

void loop() {
    if (state != TxState::SENDING) return;
    if (cursor >= frameCount) {
        // Done
        ledcWrite(PWM_CHANNEL, 0);
        state = TxState::IDLE;
        Serial.println("{\"type\":\"ir\",\"event\":\"tx_done\"}");
        return;
    }

    unsigned long now = micros();
    unsigned long elapsed = now - segmentStart;

    if (sendingPulse) {
        // Turn ON 38 kHz carrier
        ledcWrite(PWM_CHANNEL, 128); // 50% duty

        if (elapsed >= pulseBuf[cursor]) {
            // Switch to gap
            sendingPulse = false;
            segmentStart = now;
        }
    } else {
        // Turn OFF carrier
        ledcWrite(PWM_CHANNEL, 0);

        if (elapsed >= gapBuf[cursor]) {
            // Move to next pulse
            cursor++;
            sendingPulse = true;
            segmentStart = now;
        }
    }
}

void stop() {
    ledcWrite(PWM_CHANNEL, 0);
    state = TxState::OFF;

    Serial.println("{\"type\":\"sys\",\"subsystem\":\"ir_tx\",\"state\":\"off\"}");
}

void testFlash() {
    // Ensure PWM is ready
    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES);
    ledcAttachPin(IR_LED_PIN, PWM_CHANNEL);

    // Flash for 500ms
    unsigned long end = millis() + 500;
    while (millis() < end) {
        ledcWrite(PWM_CHANNEL, 128); // ON
        delay(10);
        ledcWrite(PWM_CHANNEL, 0);   // OFF
        delay(10);
    }

    ledcWrite(PWM_CHANNEL, 0);
    Serial.println("{\"type\":\"ir\",\"event\":\"test_flash\"}");
}

} // namespace IRTransmitter
