#ifndef SIM800L_HANDLER_H
#define SIM800L_HANDLER_H

#include <Arduino.h>

#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>

extern TinyGsm modem;

void setupSIM800L();
bool sendSMS(const char* number, const char* message);

#endif // SIM800L_HANDLER_H
