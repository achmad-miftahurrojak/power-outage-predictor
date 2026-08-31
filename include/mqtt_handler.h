#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <Arduino.h>

void setupWiFiAndMQTT();
void maintainMQTTConnection();
void publishTelemetry(float v_rms, float i_rms = 0.0);
void publishEvent(const char* eventType, float voltageBeforeDrop, long durationSec = 0);

#endif // MQTT_HANDLER_H
