#include "hid_ops.h"
#include "hid.h"
#include "hid_mouse.h"
#include "USBHIDKeyboard.h"
#include "USBHIDMouse.h"

#include <Arduino.h>
#include <string.h>
#include <ctype.h>

// ---------------------------------------------------------
// Keycode lookup table
// ---------------------------------------------------------

static uint8_t lookupKeycode(const char* name) {

    // Single character (letter, number, punctuation)
    if (strlen(name) == 1) {
        char c = name[0];

        // Letters
        if (c >= 'A' && c <= 'Z') return HID_KEY_A + (c - 'A');
        if (c >= 'a' && c <= 'z') return HID_KEY_A + (c - 'a');

        // Numbers (top row)
        if (c >= '0' && c <= '9') return HID_KEY_0 + (c - '0');

        // Common punctuation
        switch (c) {
            case ' ': return HID_KEY_SPACE;
            case '-': return HID_KEY_MINUS;
            case '=': return HID_KEY_EQUAL;
            case '[': return HID_KEY_BRACKET_LEFT;
            case ']': return HID_KEY_BRACKET_RIGHT;
            case '\\': return HID_KEY_BACKSLASH;
            case ';': return HID_KEY_SEMICOLON;
            case '\'': return HID_KEY_APOSTROPHE;
            case '`': return HID_KEY_GRAVE;
            case ',': return HID_KEY_COMMA;
            case '.': return HID_KEY_PERIOD;
            case '/': return HID_KEY_SLASH;
        }
    }

    // Special keys
    if (!strcmp(name, "ENTER"))     return HID_KEY_ENTER;
    if (!strcmp(name, "SPACE"))     return HID_KEY_SPACE;
    if (!strcmp(name, "TAB"))       return HID_KEY_TAB;
    if (!strcmp(name, "ESC"))       return HID_KEY_ESCAPE;
    if (!strcmp(name, "BACKSPACE")) return HID_KEY_BACKSPACE;

    // Function keys
    if (!strcmp(name, "F1"))  return HID_KEY_F1;
    if (!strcmp(name, "F2"))  return HID_KEY_F2;
    if (!strcmp(name, "F3"))  return HID_KEY_F3;
    if (!strcmp(name, "F4"))  return HID_KEY_F4;
    if (!strcmp(name, "F5"))  return HID_KEY_F5;
    if (!strcmp(name, "F6"))  return HID_KEY_F6;
    if (!strcmp(name, "F7"))  return HID_KEY_F7;
    if (!strcmp(name, "F8"))  return HID_KEY_F8;
    if (!strcmp(name, "F9"))  return HID_KEY_F9;
    if (!strcmp(name, "F10")) return HID_KEY_F10;
    if (!strcmp(name, "F11")) return HID_KEY_F11;
    if (!strcmp(name, "F12")) return HID_KEY_F12;

    return 0;
}


// ---------------------------------------------------------
// Modifier lookup
// ---------------------------------------------------------

static uint8_t lookupModifier(const char* name) {
    if (!strcmp(name, "CTRL"))  return KEYBOARD_MODIFIER_LEFTCTRL;
    if (!strcmp(name, "SHIFT")) return KEYBOARD_MODIFIER_LEFTSHIFT;
    if (!strcmp(name, "ALT"))   return KEYBOARD_MODIFIER_LEFTALT;
    if (!strcmp(name, "GUI"))   return KEYBOARD_MODIFIER_LEFTGUI;
    return 0;
}


// ---------------------------------------------------------
// Mouse button lookup
// ---------------------------------------------------------

static uint8_t lookupMouseButton(const char* name) {
    if (!strcmp(name, "LEFT"))   return MOUSE_BUTTON_LEFT;
    if (!strcmp(name, "RIGHT"))  return MOUSE_BUTTON_RIGHT;
    if (!strcmp(name, "MIDDLE")) return MOUSE_BUTTON_MIDDLE;
    return 0;
}

// ---------------------------------------------------------
// Main HID command dispatcher
// ---------------------------------------------------------

bool hid_ops_handle(const char* cmd, Stream& out) {

    // ---------------------------------------------
    // hid.key <KEYCODE>
    // ---------------------------------------------
    if (strncmp(cmd, "hid.key ", 8) == 0) {
        const char* keyName = cmd + 8;
        uint8_t key = lookupKeycode(keyName);

        if (!key) {
            out.println("{\"error\":\"unknown key\"}");
            return true;
        }

        hid::keyTap(key);
        out.println("{\"ok\":true}");
        return true;
    }

    // ---------------------------------------------
    // hid.mod <MODIFIERS> <KEYCODE>
    // Example: hid.mod CTRL SHIFT ESC
    // ---------------------------------------------
    if (strncmp(cmd, "hid.mod ", 8) == 0) {
        char buf[64];
        strncpy(buf, cmd + 8, sizeof(buf));
        buf[63] = 0;

        char* tok = strtok(buf, " ");
        uint8_t mods = 0;

        while (tok) {
            char* next = strtok(NULL, " ");

            if (!next) {
                // tok is the key
                uint8_t key = lookupKeycode(tok);
                if (!key) {
                    out.println("{\"error\":\"unknown key\"}");
                    return true;
                }
                hid::modTap(mods, key);
                out.println("{\"ok\":true}");
                return true;
            }

            uint8_t m = lookupModifier(tok);
            if (!m) {
                out.println("{\"error\":\"unknown modifier\"}");
                return true;
            }

            mods |= m;
            tok = next;
        }

        return true;
    }

    // ---------------------------------------------
    // hid.mouse.smooth dx dy
    // ---------------------------------------------
    if (strncmp(cmd, "hid.mouse.smooth ", 17) == 0) {
        int dx = 0, dy = 0;
        sscanf(cmd + 17, "%d %d", &dx, &dy);

        hid::mouseSmoothMove(dx, dy);
        out.println("{\"ok\":true}");
        return true;
    }

    // ---------------------------------------------
    // hid.mouse.drag dx dy
    // ---------------------------------------------
    if (strncmp(cmd, "hid.mouse.drag ", 15) == 0) {
        int dx = 0, dy = 0;
        sscanf(cmd + 15, "%d %d", &dx, &dy);

        hid::mouseDrag(dx, dy);
        out.println("{\"ok\":true}");
        return true;
    }

    // ---------------------------------------------
    // hid.mouse.vel dx dy speed
    // ---------------------------------------------
    if (strncmp(cmd, "hid.mouse.vel ", 14) == 0) {
        int dx = 0, dy = 0;
        float speed = 1.0f;
        sscanf(cmd + 14, "%d %d %f", &dx, &dy, &speed);

        hid::mouseMoveVelocity(dx, dy, speed);
        out.println("{\"ok\":true}");
        return true;
    }


    // ---------------------------------------------
    // hid.type <TEXT...>
    // ---------------------------------------------
    if (strncmp(cmd, "hid.type ", 9) == 0) {
        const char* text = cmd + 9;
        hid::typeText(text);
        out.println("{\"ok\":true}");
        return true;
    }

    // ---------------------------------------------
    // hid.delay <MS>
    // ---------------------------------------------
    if (strncmp(cmd, "hid.delay ", 10) == 0) {
        uint32_t ms = atoi(cmd + 10);
        hid::delayMs(ms);
        out.println("{\"ok\":true}");
        return true;
    }

    // ---------------------------------------------
    // hid.mouse.move dx dy
    // ---------------------------------------------
    if (strncmp(cmd, "hid.mouse.move ", 16) == 0) {
        int dx = 0, dy = 0;
        sscanf(cmd + 16, "%d %d", &dx, &dy);

        hid::mouseMove((int8_t)dx, (int8_t)dy);
        out.println("{\"ok\":true}");
        return true;
    }

    // ---------------------------------------------
    // hid.mouse.click BUTTON
    // ---------------------------------------------
    if (strncmp(cmd, "hid.mouse.click ", 17) == 0) {
        const char* btn = cmd + 17;
        uint8_t b = lookupMouseButton(btn);

        if (!b) {
            out.println("{\"error\":\"unknown button\"}");
            return true;
        }

        hid::mouseClick(b);
        out.println("{\"ok\":true}");
        return true;
    }

    // ---------------------------------------------
    // hid.mouse.scroll amount
    // ---------------------------------------------
    if (strncmp(cmd, "hid.mouse.scroll ", 18) == 0) {
        int amt = atoi(cmd + 18);
        hid::mouseScroll((int8_t)amt);
        out.println("{\"ok\":true}");
        return true;
    }
    
    // ---------------------------------------------
    // hid.exec <DELAY_MS> <TEXT...>
    // Example: hid.exec 2000 Hello World
    // ---------------------------------------------
    if (strncmp(cmd, "hid.exec ", 9) == 0) {

        // Find the first space after "hid.exec "
        const char* rest = cmd + 9;
        const char* space = strchr(rest, ' ');

        if (!space) {
            out.println("{\"error\":\"bad_format\"}");
            return true;
        }

        // Parse delay
        int delayMs = atoi(rest);

        // Extract text after delay
        const char* text = space + 1;

        // Emit pending event
        out.printf("{\"hid\":\"pending\",\"delay_ms\":%d}\n", delayMs);

        // Wait
        delay(delayMs);

        // Emit typing event
        out.printf("{\"hid\":\"typing\",\"text\":\"%s\"}\n", text);

        // Perform HID typing
        hid::typeText(text);

        out.println("{\"ok\":true}");
        return true;
    }


    return false; // not a HID command
}
