#include <Arduino.h>
#include "button.h"

static const int BUTTON_PIN = 17;

// Timing thresholds (ms)
static const unsigned long DEBOUNCE_MS       = 40;
static const unsigned long SHORT_PRESS_MAX   = 500;
static const unsigned long LONG_PRESS_MIN    = 500;
static const unsigned long HOLD_PRESS_MIN    = 10000;
static const unsigned long DOUBLE_TAP_WINDOW = 300;

static bool pressed = false;
static bool holdFired = false;
static bool suppressNextShort = false;

static unsigned long pressStart = 0;
static unsigned long lastShortPressTime = 0;
static unsigned long lastChangeTime = 0;

namespace Button {

void init() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
    unsigned long now = millis();
    bool rawPressed = (digitalRead(BUTTON_PIN) == LOW);

    // ------------------------------------------------------------
    // 1. Debounce input
    // ------------------------------------------------------------
    static bool stablePressed = false;

    if (rawPressed != stablePressed && (now - lastChangeTime) > DEBOUNCE_MS) {
        stablePressed = rawPressed;
        lastChangeTime = now;
    }

    bool isPressed = stablePressed;

    // ------------------------------------------------------------
    // 2. Detect press start
    // ------------------------------------------------------------
    if (isPressed && !pressed) {
        pressed = true;
        holdFired = false;
        suppressNextShort = false;
        pressStart = now;
    }

    // ------------------------------------------------------------
    // 3. Detect HOLD while still pressed
    // ------------------------------------------------------------
    if (pressed && isPressed) {
        unsigned long duration = now - pressStart;

        if (!holdFired && duration >= HOLD_PRESS_MIN) {
            Serial.println("{\"button\":\"hold_press\"}");
            holdFired = true;
            suppressNextShort = true;  // suppress all other events
        }
    }

    // ------------------------------------------------------------
    // 4. Detect release
    // ------------------------------------------------------------
    if (!isPressed && pressed) {
        pressed = false;
        unsigned long duration = now - pressStart;

        // If hold fired, ignore release
        if (holdFired) {
            holdFired = false;
            return;
        }

        // LONG press
        if (duration >= LONG_PRESS_MIN) {
            Serial.println("{\"button\":\"long_press\"}");
            suppressNextShort = true;
            return;
        }

        // SHORT press candidate
        if (duration < SHORT_PRESS_MAX) {
            if (suppressNextShort) {
                // double_press or hold already handled
                return;
            }

            if (now - lastShortPressTime <= DOUBLE_TAP_WINDOW) {
                Serial.println("{\"button\":\"double_press\"}");
                lastShortPressTime = 0;
                suppressNextShort = true;  // prevent trailing short_press
                return;
            } else {
                lastShortPressTime = now;
            }
        }
    }

    // ------------------------------------------------------------
    // 5. Emit short_press if double-tap window expires
    // ------------------------------------------------------------
    if (!pressed &&
        lastShortPressTime != 0 &&
        (now - lastShortPressTime > DOUBLE_TAP_WINDOW)) {

        if (!suppressNextShort) {
            Serial.println("{\"button\":\"short_press\"}");
        }

        lastShortPressTime = 0;
        suppressNextShort = false;
    }
}

}
