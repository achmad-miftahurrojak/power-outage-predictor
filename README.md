# Power Outage Predictor

An early warning system designed to detect power grid instability. By continuously monitoring AC voltage, this system identifies severe brownout patterns that often precede a complete grid failure. Once a critical voltage drop is confirmed, it dispatches an SMS alert via a GSM module.

## Features

*   **AC Voltage Monitoring:** Safely calculates the Root Mean Square (RMS) of AC voltage using the ZMPT101B sensor.
*   **Brownout Detection:** Identifies and confirms dangerous voltage drops (under 180V) before classifying them as grid failures.
*   **Cellular Alerts:** Integrates with a SIM800L GSM module to send SMS notifications using standard AT commands.
*   **Fault Tolerance:** Utilizes the ESP32 Hardware Watchdog Timer to automatically reboot the system if the GSM connection hangs.

## Hardware Requirements

*   ESP32 Development Board
*   ZMPT101B AC Voltage Sensor
*   SIM800L GSM Module

## Software Stack

*   **Environment:** PlatformIO / Arduino framework
*   **Language:** C++
*   **Libraries:** `HardwareSerial`, `esp_task_wdt`

## Setup Instructions

1.  Open the project in PlatformIO.
2.  Adjust the `VOLTAGE_THRESHOLD` and target phone number in `src/main.cpp`.
3.  Upload the firmware to your ESP32 board.
4.  Provide adequate power to the SIM800L module (requires up to 2A during transmission).
