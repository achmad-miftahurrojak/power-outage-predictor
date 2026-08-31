#ifndef HW_TESTS_H
#define HW_TESTS_H

#include <Arduino.h>
#include "config.h"
#include "secrets.h"
#include "sd_logger.h"
#include "sim800l_handler.h"
#include "mqtt_handler.h"
#include <ZMPT101B.h>

// ======================================================================
// UNCOMMENT ONE OF THE LINES BELOW TO RUN AN ISOLATED HARDWARE TEST
// ======================================================================
// #define TEST_HW_RTC
// #define TEST_HW_SD
// #define TEST_HW_SIM800L
// #define TEST_HW_WIFI_MQTT
// #define TEST_HW_ZMPT101B

void runHardwareTestsIfEnabled() {
#if defined(TEST_HW_RTC)
    Serial.begin(115200);
    setupSDAndRTC();
    while (true) {
        Serial.println("RTC Time: " + getCurrentTimestamp());
        delay(1000);
    }

#elif defined(TEST_HW_SD)
    Serial.begin(115200);
    setupSDAndRTC();
    Serial.println("Writing test line to SD card...");
    logDataToSD(220.0, 0.0, "TEST_STATE", "TEST_EVENT");
    Serial.println("Check the SD card for power_log.csv");
    while (true) delay(1000);

#elif defined(TEST_HW_SIM800L)
    Serial.begin(115200);
    setupSIM800L();
    Serial.println("SIM800L Init complete. Checking CREG...");
    modem.sendAT("+CREG?");
    if (modem.waitResponse(10000L) == 1) {
        Serial.println("CREG response received.");
    }
    Serial.println("Attempting to send a test SMS in 5 seconds...");
    delay(5000);
    sendSMS(SMS_TARGET_NUMBER, "[TEST] SMS from Power Outage Predictor");
    while (true) delay(1000);

#elif defined(TEST_HW_WIFI_MQTT)
    Serial.begin(115200);
    setupWiFiAndMQTT();
    while (true) {
        maintainMQTTConnection();
        Serial.println("Publishing dummy payload...");
        publishTelemetry(225.5, 0.0);
        delay(5000);
    }

#elif defined(TEST_HW_ZMPT101B)
    Serial.begin(115200);
    ZMPT101B testSensor(PIN_ZMPT101B);
    testSensor.setSensitivity(ZMPT101B_CALIBRATION);
    while (true) {
        float rawRms = testSensor.getRmsVoltage();
        Serial.print("Raw RMS Voltage: ");
        Serial.println(rawRms);
        delay(1000);
    }
#endif
}

#endif // HW_TESTS_H
