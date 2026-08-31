#include <Arduino.h>
#include "config.h"
#include "secrets.h"
#include "state_machine.h"
#include "sim800l_handler.h"
#include "mqtt_handler.h"
#include "sd_logger.h"
#include <ZMPT101B.h>
#include "hw_tests.h"

ZMPT101B zmpt101b(PIN_ZMPT101B);
StateMachine stateMachine;

unsigned long lastRmsEvalTime = 0;
unsigned long lastNormalLogTime = 0;
unsigned long lastAlertLogTime = 0;
long outageStartTime = 0;

void setLEDs(SystemState state) {
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_YELLOW, LOW);
    digitalWrite(PIN_LED_RED, LOW);
    
    switch (state) {
        case STATE_NORMAL:
        case STATE_RESTORED:
            digitalWrite(PIN_LED_GREEN, HIGH);
            break;
        case STATE_WARNING:
            digitalWrite(PIN_LED_YELLOW, HIGH);
            break;
        case STATE_OUTAGE:
            digitalWrite(PIN_LED_RED, HIGH);
            break;
    }
}

void triggerBuzzer(bool active) {
    digitalWrite(PIN_BUZZER, active ? HIGH : LOW);
}

void setup() {
    runHardwareTestsIfEnabled(); // Will hijack execution if a test macro is defined in hw_tests.h
    
    Serial.begin(115200);
    while (!Serial);

    Serial.println("Starting Power Outage Predictor...");

    // Setup GPIOs
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_YELLOW, OUTPUT);
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);
    setLEDs(STATE_NORMAL);
    triggerBuzzer(false);

    // Initialize subsystems
    setupSDAndRTC();
    setupWiFiAndMQTT();
    setupSIM800L();

    // Initialize ZMPT101B
    zmpt101b.setSensitivity(ZMPT101B_CALIBRATION);

    Serial.println("Initialization complete. Entering main loop.");
}

void loop() {
    // Keep MQTT connection alive
    maintainMQTTConnection();

    unsigned long currentMillis = millis();

    // Evaluate state every RMS_WINDOW_MS (1 second)
    if (currentMillis - lastRmsEvalTime >= RMS_WINDOW_MS) {
        lastRmsEvalTime = currentMillis;

        float currentVoltage = zmpt101b.getRmsVoltage();

        bool stateChanged = stateMachine.evaluate(currentVoltage);
        SystemState currentState = stateMachine.getCurrentState();

        setLEDs(currentState);

        if (stateChanged) {
            String smsMsg = "";
            const char* eventType = "";
            float vDrop = stateMachine.getVoltageBeforeDrop();

            switch (currentState) {
                case STATE_WARNING:
                    eventType = "pre_failure_pattern";
                    Serial.println("STATE: WARNING");
                    break;
                case STATE_OUTAGE:
                    eventType = "outage_start";
                    outageStartTime = getUnixTime();
                    triggerBuzzer(true);
                    Serial.println("STATE: OUTAGE");
                    
                    smsMsg = "[ALERT] Listrik padam! Tegangan sblm drop: " + String(vDrop) + "V.";
                    sendSMS(SMS_TARGET_NUMBER, smsMsg.c_str());
                    publishEvent(eventType, vDrop, 0);
                    break;
                case STATE_RESTORED: {
                    eventType = "outage_end";
                    triggerBuzzer(false);
                    long duration = getUnixTime() - outageStartTime;
                    Serial.println("STATE: RESTORED");

                    smsMsg = "[INFO] Listrik nyala. Durasi padam: " + String(duration / 60) + " mnt.";
                    sendSMS(SMS_TARGET_NUMBER, smsMsg.c_str());
                    publishEvent(eventType, 0.0, duration);
                    break;
                }
                case STATE_NORMAL:
                    eventType = "normal";
                    triggerBuzzer(false);
                    Serial.println("STATE: NORMAL");
                    break;
            }

            // Immediately log event to SD
            if (currentState != STATE_NORMAL) {
                logDataToSD(currentVoltage, 0.0, String(currentState).c_str(), eventType);
                lastAlertLogTime = currentMillis;
            }
        }

        // Handle periodic logging based on current state
        if (currentState == STATE_NORMAL || currentState == STATE_RESTORED) {
            if (currentMillis - lastNormalLogTime >= TELEMETRY_NORMAL_INTERVAL_MS) {
                lastNormalLogTime = currentMillis;
                publishTelemetry(currentVoltage, 0.0);
            }
            if (currentMillis - lastNormalLogTime >= LOG_NORMAL_INTERVAL_MS) { // Use same timer variable or separate
                logDataToSD(currentVoltage, 0.0, "NORMAL");
            }
        } else {
            // High frequency logging for WARNING/OUTAGE
            if (currentMillis - lastAlertLogTime >= LOG_ALERT_INTERVAL_MS) {
                lastAlertLogTime = currentMillis;
                logDataToSD(currentVoltage, 0.0, (currentState == STATE_WARNING) ? "WARNING" : "OUTAGE");
            }
        }
    }
}
