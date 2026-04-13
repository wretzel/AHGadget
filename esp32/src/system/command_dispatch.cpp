#include "command_dispatch.h"
#include "subsystem_manager.h"
#include "../ir/ir_transmitter.h"
#include "../msc/msc_commands.h"
#include "../hid/hid_ops.h" 
#include "../rf/rf.h"

namespace CommandDispatch {

void handleCommand(const String& cmd) {

    if (cmd == "ble on") {
        SubsystemManager::enableBLE();
        // BLE is now scanning
        Serial.println("{\"type\":\"sys\",\"subsystem\":\"ble\",\"state\":\"scanning\"}");
    }

    else if (cmd == "ble off") {
        SubsystemManager::disableBLE();
        // BLE is now off
        Serial.println("{\"type\":\"sys\",\"subsystem\":\"ble\",\"state\":\"off\"}");
    }

    else if (cmd == "ble status") {
        bool active = SubsystemManager::isBLEEnabled();
        // Map ON → scanning, OFF → off
        Serial.printf(
            "{\"type\":\"sys\",\"subsystem\":\"ble\",\"state\":\"%s\"}\n",
            active ? "scanning" : "off"
        );
    }

    else if (cmd.startsWith("ir_tx_raw")) {

        int pIndex = cmd.indexOf("pulses:");
        int gIndex = cmd.indexOf("gaps:");

        if (pIndex == -1 || gIndex == -1) {
            Serial.println("{\"type\":\"error\",\"msg\":\"invalid ir_tx_raw format\"}");
            return;
        }

        // Extract and trim
        String pulseStr = cmd.substring(pIndex + 7, gIndex);
        pulseStr.trim();

        String gapStr = cmd.substring(gIndex + 5);
        gapStr.trim();

        static uint16_t pulses[400];
        static uint16_t gaps[400];
        size_t count = 0;

        // Parse pulses
        int start = 0;
        while (true) {
            int comma = pulseStr.indexOf(',', start);
            String token = (comma == -1)
                ? pulseStr.substring(start)
                : pulseStr.substring(start, comma);

            pulses[count] = token.toInt();
            count++;

            if (comma == -1) break;
            start = comma + 1;
        }

        // Parse gaps
        start = 0;
        size_t gapCount = 0;
        while (true) {
            int comma = gapStr.indexOf(',', start);
            String token = (comma == -1)
                ? gapStr.substring(start)
                : gapStr.substring(start, comma);

            gaps[gapCount] = token.toInt();
            gapCount++;

            if (comma == -1) break;
            start = comma + 1;
        }

        if (gapCount != count) {
            Serial.println("{\"type\":\"error\",\"msg\":\"pulse/gap count mismatch\"}");
            return;
        }

        IRTransmitter::loadFrame(pulses, gaps, count);

        Serial.println("{\"type\":\"ir\",\"event\":\"tx_cmd\",\"count\":" + String(count) + "}");
    }

        else if (cmd == "ir on") {
        SubsystemManager::enableIR();
        Serial.println("{\"type\":\"sys\",\"subsystem\":\"ir\",\"state\":\"on\"}");
    }

    else if (cmd == "ir off") {
        SubsystemManager::disableIR();
        Serial.println("{\"type\":\"sys\",\"subsystem\":\"ir\",\"state\":\"off\"}");
    }

    else if (cmd == "ir status") {
        bool active = SubsystemManager::isIREnabled();
        Serial.printf(
            "{\"type\":\"sys\",\"subsystem\":\"ir\",\"state\":\"%s\"}\n",
            active ? "on" : "off"
        );
    }
    
    else if (cmd == "rf on") {
        SubsystemManager::enableRF();
        Serial.println("{\"type\":\"sys\",\"subsystem\":\"rf\",\"state\":\"on\"}");
    }

    else if (cmd == "rf off") {
        SubsystemManager::disableRF();
        Serial.println("{\"type\":\"sys\",\"subsystem\":\"rf\",\"state\":\"off\"}");
    }

    else if (cmd == "rf status") {
        bool active = SubsystemManager::isRFEnabled();
        Serial.printf(
            "{\"type\":\"sys\",\"subsystem\":\"rf\",\"state\":\"%s\"}\n",
            active ? "on" : "off"
        );
    }


    // -----------------------------
    // RF COMMANDS
    // -----------------------------

    else if (cmd.startsWith("rf_tx_raw")) {

        int pIndex = cmd.indexOf("pulses:");
        int gIndex = cmd.indexOf("gaps:");

        if (pIndex == -1 || gIndex == -1) {
            Serial.println("{\"type\":\"error\",\"msg\":\"invalid rf_tx_raw format\"}");
            return;
        }

        // Extract and trim
        String pulseStr = cmd.substring(pIndex + 7, gIndex);
        pulseStr.trim();

        String gapStr = cmd.substring(gIndex + 5);
        gapStr.trim();

        static RFFrame frame;
        frame.count = 0;
        frame.valid = true;

        // Parse pulses
        int start = 0;
        while (true) {
            int comma = pulseStr.indexOf(',', start);
            String token = (comma == -1)
                ? pulseStr.substring(start)
                : pulseStr.substring(start, comma);

            frame.pulses[frame.count] = token.toInt();
            frame.count++;

            if (comma == -1) break;
            start = comma + 1;
        }

        // Parse gaps
        start = 0;
        size_t gapCount = 0;
        while (true) {
            int comma = gapStr.indexOf(',', start);
            String token = (comma == -1)
                ? gapStr.substring(start)
                : gapStr.substring(start, comma);

            frame.gaps[gapCount] = token.toInt();
            gapCount++;

            if (comma == -1) break;
            start = comma + 1;
        }

        if (gapCount != frame.count) {
            Serial.println("{\"type\":\"error\",\"msg\":\"pulse/gap count mismatch\"}");
            return;
        }

        RF::transmit(frame);

        Serial.println("{\"type\":\"rf\",\"event\":\"tx_cmd\",\"count\":" + String(frame.count) + "}");
    }

    // -----------------------------
    // MSC COMMANDS
    // -----------------------------
    else if (cmd.startsWith("msc.write ")) {
        // Format: msc.write filename base64data
        int firstSpace = cmd.indexOf(' ');
        int secondSpace = cmd.indexOf(' ', firstSpace + 1);

        if (secondSpace == -1) {
            Serial.println("{\"type\":\"error\",\"msg\":\"invalid msc.write format\"}");
            return;
        }

        String filename = cmd.substring(firstSpace + 1, secondSpace);
        String base64   = cmd.substring(secondSpace + 1);

        if (msc::writeFile(filename, base64)) {
            Serial.println("{\"type\":\"msc\",\"event\":\"write\",\"file\":\"" + filename + "\"}");
        } else {
            Serial.println("{\"type\":\"error\",\"msg\":\"msc.write failed\"}");
        }
    }

    else if (cmd.startsWith("msc.read ")) {
        // Format: msc.read filename
        String filename = cmd.substring(9);
        filename.trim();

        Serial.print("{\"type\":\"msc\",\"event\":\"read\",\"file\":\"");
        Serial.print(filename);
        Serial.print("\",\"data\":\"");

        if (!msc::readFile(filename, Serial)) {
            Serial.println("\",\"error\":\"read failed\"}");
            return;
        }

        Serial.println("\"}");
    }

    else if (cmd == "msc.ls") {
        Serial.print("{\"type\":\"msc\",\"event\":\"ls\",");
        msc::listFiles(Serial);
        Serial.println("}");
    }

    else if (cmd.startsWith("msc.rm ")) {
        String filename = cmd.substring(7);
        filename.trim();

        if (msc::deleteFile(filename)) {
            Serial.println("{\"type\":\"msc\",\"event\":\"rm\",\"file\":\"" + filename + "\"}");
        } else {
            Serial.println("{\"type\":\"error\",\"msg\":\"msc.rm failed\"}");
        }
    }

    else if (cmd == "msc.format") {
        if (msc::formatDisk()) {
            Serial.println("{\"type\":\"msc\",\"event\":\"format\",\"status\":\"ok\"}");
        } else {
            Serial.println("{\"type\":\"error\",\"msg\":\"msc.format failed\"}");
        }
    }

    else if (cmd == "msc.info") {
        Serial.print("{\"type\":\"msc\",\"event\":\"info\",");
        msc::info(Serial);
        Serial.println("}");
    }

    // -----------------------------
    // HID COMMANDS (keyboard + mouse)
    // -----------------------------
    else if (cmd.startsWith("hid.")) {
        if (hid_ops_handle(cmd.c_str(), Serial)) {
            // hid_ops already printed {"ok":true} or an error
            return;
        } else {
            Serial.println("{\"type\":\"error\",\"msg\":\"invalid hid command\"}");
            return;
        }
    }

    else if (cmd == "ir test") {
        IRTransmitter::testFlash();
    }

    else {
        Serial.println("{\"type\":\"error\",\"msg\":\"unknown command\"}");
    }
    
}

}
