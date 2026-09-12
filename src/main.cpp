#include <Arduino.h>
#include <HardwareSerial.h>
#include <esp_task_wdt.h>

#define PIN_ZMPT101B 34
#define SIM_TX 17
#define SIM_RX 16

const int VOLTAGE_THRESHOLD = 180;
const int BROWN_OUT_CONFIRMATION_TIME = 2000;

HardwareSerial sim800l(2);

unsigned long brownout_start_time = 0;
bool is_outage = false;

float getACVoltage();
void sendSMSAlert(String message);

void setup() {
  Serial.begin(115200);
  sim800l.begin(9600, SERIAL_8N1, SIM_RX, SIM_TX);
  
  Serial.println("Power Outage Predictor Init...");

  esp_task_wdt_init(10, true);
  esp_task_wdt_add(NULL);

  delay(5000); 
  sim800l.println("AT");
}

void loop() {
  esp_task_wdt_reset();

  float voltage = getACVoltage();
  Serial.printf("AC Voltage: %.2f V\n", voltage);

  if (voltage < VOLTAGE_THRESHOLD) {
    if (brownout_start_time == 0) {
      brownout_start_time = millis();
    } else if (millis() - brownout_start_time > BROWN_OUT_CONFIRMATION_TIME && !is_outage) {
      is_outage = true;
      Serial.println("ALERT: OUTAGE DETECTED!");
      sendSMSAlert("ALERT: Power Grid Failure Detected! Voltage dropped below 180V.");
    }
  } else {
    if (is_outage) {
      Serial.println("Power restored.");
      sendSMSAlert("INFO: Power Grid Restored.");
    }
    is_outage = false;
    brownout_start_time = 0;
  }

  delay(50);
}

float getACVoltage() {
  uint32_t sum_sq = 0;
  int n = 0;
  unsigned long start = millis();
  
  while (millis() - start < 20) {
    int val = analogRead(PIN_ZMPT101B) - 2048;
    sum_sq += val * val;
    n++;
  }
  
  if (n == 0) return 0;
  float rms = sqrt(sum_sq / n);
  float voltage = rms * 0.45; 
  return voltage;
}

void sendSMSAlert(String message) {
  sim800l.println("AT+CMGF=1");
  delay(100);
  sim800l.println("AT+CMGS=\"+6281234567890\"");
  delay(100);
  sim800l.print(message);
  delay(100);
  sim800l.write(26);
  delay(1000);
}
