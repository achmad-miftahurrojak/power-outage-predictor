#include "sd_logger.h"
#include "config.h"
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;
bool sdAvailable = false;
bool rtcAvailable = false;

const char* logFileName = "/power_log.csv";

void setupSDAndRTC() {
    Serial.println("Initializing RTC...");
    // Initialize I2C for RTC
    Wire.begin(); 
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
    } else {
        rtcAvailable = true;
        if (rtc.lostPower()) {
            Serial.println("RTC lost power, let's set the time!");
            // This will set the RTC to the date & time this sketch was compiled
            rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        }
        Serial.println("RTC Initialized.");
    }

    Serial.println("Initializing SD Card...");
    // Initialize SD Card
    if (!SD.begin(PIN_SD_CS)) {
        Serial.println("Card Mount Failed");
    } else {
        sdAvailable = true;
        Serial.println("SD Card Initialized.");
        
        // Write CSV header if file doesn't exist
        if (!SD.exists(logFileName)) {
            File file = SD.open(logFileName, FILE_WRITE);
            if (file) {
                file.println("timestamp,v_rms,i_rms,state,event_type");
                file.close();
            }
        }
    }
}

String getCurrentTimestamp() {
    if (!rtcAvailable) return "1970-01-01T00:00:00";
    
    DateTime now = rtc.now();
    char buf[25];
    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d", 
            now.year(), now.month(), now.day(),
            now.hour(), now.minute(), now.second());
    return String(buf);
}

long getUnixTime() {
    if (!rtcAvailable) return 0;
    return rtc.now().unixtime();
}

void logDataToSD(float v_rms, float i_rms, const char* stateStr, const char* eventTypeStr) {
    if (!sdAvailable) return;

    File file = SD.open(logFileName, FILE_APPEND);
    if (file) {
        String timestamp = getCurrentTimestamp();
        file.print(timestamp);
        file.print(",");
        file.print(v_rms);
        file.print(",");
        file.print(i_rms);
        file.print(",");
        file.print(stateStr);
        file.print(",");
        file.println(eventTypeStr);
        file.close();
        Serial.println("Logged to SD.");
    } else {
        Serial.println("Failed to open file for appending");
    }
}
