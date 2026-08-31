#include "sim800l_handler.h"
#include "config.h"

// Use HardwareSerial for UART2
HardwareSerial SerialAT(2);
TinyGsm modem(SerialAT);

void setupSIM800L() {
    Serial.println("Initializing SIM800L...");
    // Initialize UART2 for SIM800L
    SerialAT.begin(9600, SERIAL_8N1, PIN_SIM_RX, PIN_SIM_TX);
    
    // Restart takes quite some time
    delay(3000);
    
    Serial.println("Initializing modem...");
    if (!modem.restart()) {
        Serial.println("Failed to restart modem, attempting to continue without restarting");
    }

    String modemInfo = modem.getModemInfo();
    Serial.print("Modem Info: ");
    Serial.println(modemInfo);

    Serial.print("Waiting for network...");
    if (!modem.waitForNetwork()) {
        Serial.println(" fail");
        return;
    }
    Serial.println(" success");

    if (modem.isNetworkConnected()) {
        Serial.println("Network connected");
    }
}

bool sendSMS(const char* number, const char* message) {
    Serial.print("Sending SMS to ");
    Serial.print(number);
    Serial.println("...");
    
    bool res = modem.sendSMS(number, String(message));
    if (res) {
        Serial.println("SMS sent successfully.");
    } else {
        Serial.println("SMS failed to send.");
    }
    return res;
}
