#include "mqtt_handler.h"
#include "secrets.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);


const char* telemetryTopic = "desa/desa_contoh/power/device_01/telemetry";
const char* eventTopic = "desa/desa_contoh/power/device_01/event";
const char* statusTopic = "desa/desa_contoh/power/device_01/status";

void setupWiFiAndMQTT() {
    Serial.print("Connecting to WiFi: ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
        delay(500);
        Serial.print(".");
    }
    
    if(WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected.");
    } else {
        Serial.println("\nWiFi connection failed. Will retry later.");
    }

    espClient.setInsecure(); 
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

    mqttClient.setBufferSize(512);
}

void maintainMQTTConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        return; 
    }

    if (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS, statusTopic, 1, true, "offline")) {
            Serial.println("connected");
            mqttClient.publish(statusTopic, "online", true);
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again later");
        }
    }
    mqttClient.loop();
}

void publishTelemetry(float v_rms, float i_rms) {
    if (!mqttClient.connected()) return;

    StaticJsonDocument<200> doc;

    doc["v_avg"] = v_rms;
    doc["i_avg"] = i_rms;

    char buffer[200];
    serializeJson(doc, buffer);
    
    mqttClient.publish(telemetryTopic, buffer);
    Serial.println("Published telemetry via MQTT.");
}

void publishEvent(const char* eventType, float voltageBeforeDrop, long durationSec) {
    if (!mqttClient.connected()) return;

    StaticJsonDocument<256> doc;
    doc["type"] = eventType;
    if (voltageBeforeDrop > 0.0) {
        doc["v_before_drop"] = voltageBeforeDrop;
    }
    if (durationSec > 0) {
        doc["duration_sec"] = durationSec;
    }

    char buffer[256];
    serializeJson(doc, buffer);
    
    mqttClient.publish(eventTopic, buffer);
    Serial.print("Published event via MQTT: ");
    Serial.println(eventType);
}
