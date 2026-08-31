#ifndef CONFIG_H
#define CONFIG_H

// --- Hardware Pins Configuration ---

// Analog Sensors (MUST use ADC1 pins, ADC2 conflicts with WiFi)
#define PIN_ZMPT101B 32 // ADC1_CH4
// #define PIN_ACS712 33 // ADC1_CH5 (Reserved for future)

// SIM800L (GSM) - UART2
#define PIN_SIM_RX 16
#define PIN_SIM_TX 17

// SD Card - VSPI Default
#define PIN_SD_CS 5
// MISO=19, MOSI=23, SCK=18 (Hardware defaults)

// I2C - RTC DS3231 Default
// SDA=21, SCL=22 (Hardware defaults)

// Actuators & Indicators
#define PIN_BUZZER 4
#define PIN_LED_GREEN 13   // NORMAL
#define PIN_LED_YELLOW 14  // WARNING
#define PIN_LED_RED 27     // OUTAGE

// --- System Thresholds & Parameters ---

#define VOLTAGE_NORMAL_MIN 200.0 // Volts
#define VOLTAGE_WARNING_DROP 20.0 // % drop
#define VOLTAGE_OUTAGE_MAX 10.0 // Volts (considered outage)

// Sampling & Timing
#define ZMPT101B_CALIBRATION 500.0 // Adjust based on calibration
#define ZMPT101B_FREQUENCY 50 // Hz
#define RMS_WINDOW_MS 1000 // Evaluate RMS every 1000ms

#define TELEMETRY_NORMAL_INTERVAL_MS 60000 // 60s
#define LOG_NORMAL_INTERVAL_MS 30000 // 30s
#define LOG_ALERT_INTERVAL_MS 1000 // 1s

#endif // CONFIG_H
