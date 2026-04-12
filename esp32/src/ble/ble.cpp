#include <Arduino.h>

extern "C" {
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
}

enum class BLEState {
    OFF,
    INITIALIZED,
    SCANNING
};

static BLEState state = BLEState::OFF;

// Convert bytes to hex string
static String toHex(const uint8_t *data, uint8_t len) {
    String out;
    for (int i = 0; i < len; i++) {
        char buf[3];
        sprintf(buf, "%02X", data[i]);
        out += buf;
    }
    return out;
}

// GAP callback
static void gap_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    if (event != ESP_GAP_BLE_SCAN_RESULT_EVT) return;

    auto &r = param->scan_rst;

    // Only process actual advertisement results
    if (r.search_evt != ESP_GAP_SEARCH_INQ_RES_EVT) return;

    // MAC address
    char mac[18];
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
        r.bda[0], r.bda[1], r.bda[2],
        r.bda[3], r.bda[4], r.bda[5]);

    // Raw advertisement payload
    String payload_hex = toHex(r.ble_adv, r.adv_data_len);

    // Address type mapping
    const char *addr_type =
        (r.ble_addr_type == BLE_ADDR_TYPE_PUBLIC) ? "public" : "random";

    // Advertisement type mapping
    const char *adv_type =
        (r.ble_evt_type == ESP_BLE_EVT_CONN_ADV) ? "connectable" : "non_connectable";

    // JSON output (L0/L1 compatible)
    String json = "{";
    json += "\"type\":\"ble\",";
    json += "\"event\":\"adv\",";
    json += "\"ts\":" + String(millis()) + ",";
    json += "\"addr\":\"" + String(mac) + "\",";
    json += "\"addr_type\":\"" + String(addr_type) + "\",";
    json += "\"rssi\":" + String(r.rssi) + ",";
    json += "\"adv_type\":\"" + String(adv_type) + "\",";
    json += "\"payload_hex\":\"" + payload_hex + "\"";
    json += "}";

    Serial.println(json);
}

void ble_init() {
    if (state != BLEState::OFF) {
        Serial.println("{\"type\":\"sys\",\"subsystem\":\"ble\",\"state\":\"off\"}");
        return;
    }

    esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);

    esp_bluedroid_init();
    esp_bluedroid_enable();

    esp_ble_gap_register_callback(gap_callback);

    state = BLEState::INITIALIZED;

    // SysEvent: BLE initialized but not scanning
    Serial.println("{\"type\":\"sys\",\"subsystem\":\"ble\",\"state\":\"off\"}");
}

void ble_loop() {
    if (state != BLEState::INITIALIZED) return;

    // Start scanning once
    esp_ble_scan_params_t scan_params = {
        .scan_type              = BLE_SCAN_TYPE_ACTIVE,
        .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
        .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
        .scan_interval          = 0x50,
        .scan_window            = 0x30,
        .scan_duplicate         = BLE_SCAN_DUPLICATE_DISABLE
    };

    esp_ble_gap_set_scan_params(&scan_params);
    esp_ble_gap_start_scanning(0);

    state = BLEState::SCANNING;

    // Emit scan_start event
    Serial.println("{\"type\":\"ble\",\"event\":\"scan_start\",\"ts\":" + String(millis()) + "}");

    // SysEvent: now scanning
    Serial.println("{\"type\":\"sys\",\"subsystem\":\"ble\",\"state\":\"scanning\"}");
}

void ble_stop() {
    if (state == BLEState::SCANNING) {
        esp_ble_gap_stop_scanning();

        // Emit scan_stop event
        Serial.println("{\"type\":\"ble\",\"event\":\"scan_stop\",\"ts\":" + String(millis()) + ",\"reason\":\"manual\"}");
    }

    esp_bluedroid_disable();
    esp_bluedroid_deinit();

    state = BLEState::OFF;

    // SysEvent: BLE is now off
    Serial.println("{\"type\":\"sys\",\"subsystem\":\"ble\",\"state\":\"off\"}");
}
