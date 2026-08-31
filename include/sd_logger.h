#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include <Arduino.h>

void setupSDAndRTC();
void logDataToSD(float v_rms, float i_rms, const char* stateStr, const char* eventTypeStr = "");
String getCurrentTimestamp();
long getUnixTime();

#endif // SD_LOGGER_H
