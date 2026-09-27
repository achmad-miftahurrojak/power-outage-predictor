#include <Arduino.h>
#include <esp_task_wdt.h>

#include "config.h"
#include "sd_logger.h"
#include "secrets.h"
#include "sim800l_handler.h"
#include "state_machine.h"

#if __has_include("secrets.h")
#include "secrets.h"
#else
static const char* SMS_TARGET_NUMBER = "+620000000000";
#endif

static StateMachine stateMachine;
static uint32_t lastLogAt = 0;
static uint32_t lastSmsAt = 0;
static bool smsSent = false;

static void sendRateLimitedSms(const char* message) {
    if (smsSent && millis() - lastSmsAt < ALERT_COOLDOWN_MS) {
        Serial.println("SMS suppressed by alert cooldown.");
        return;
    }
    if (sendSMS(SMS_TARGET_NUMBER, message)) {
        lastSmsAt = millis();
        smsSent = true;
    }
}

static float getACVoltage() {
    double sumSq = 0.0;
    uint32_t samples = 0;
    const uint32_t startedAt = millis();

    while (millis() - startedAt < VOLTAGE_SAMPLE_WINDOW_MS) {
        const float centered = static_cast<float>(analogRead(PIN_ZMPT101B)) - 2048.0f;
        sumSq += static_cast<double>(centered) * static_cast<double>(centered);
        samples++;
    }

    if (samples == 0) return 0.0f;
    const float adcRms = sqrt(static_cast<float>(sumSq / samples));
    return adcRms * ZMPT101B_SCALE;
}

void setup() {
    Serial.begin(115200);
    Serial.println("Power Outage Predictor Init...");

    esp_task_wdt_init(10, true);
    esp_task_wdt_add(NULL);
    setupSDAndRTC();
    setupSIM800L();
}

void loop() {
    esp_task_wdt_reset();

    const float voltage = getACVoltage();
    Serial.printf("AC Voltage: %.2f V\n", voltage);

    if (stateMachine.evaluate(voltage)) {
        const SystemState state = stateMachine.getCurrentState();
        if (state == STATE_WARNING) {
            Serial.println("WARNING: VOLTAGE DROP DETECTED!");
            sendRateLimitedSms("WARNING: Grid voltage drop detected.");
            logDataToSD(voltage, 0.0f, "WARNING", "VOLTAGE_DROP");
        } else if (state == STATE_OUTAGE) {
            Serial.println("ALERT: OUTAGE DETECTED!");
            sendRateLimitedSms("ALERT: Power grid outage detected.");
            logDataToSD(voltage, 0.0f, "OUTAGE", "OUTAGE_DETECTED");
        } else if (state == STATE_RESTORED) {
            Serial.println("Power restored.");
            sendRateLimitedSms("INFO: Power grid restored.");
            logDataToSD(voltage, 0.0f, "RESTORED", "POWER_RESTORED");
        }
    }

    if (millis() - lastLogAt >= LOG_NORMAL_INTERVAL_MS) {
        lastLogAt = millis();
        logDataToSD(voltage, 0.0f, "NORMAL", "SAMPLE");
    }

    delay(50);
}
