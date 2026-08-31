# Power Outage Predictor

ESP32-based electrical monitoring and outage detection system for early warning and remote notification.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-ESP32-blue.svg)](https://platformio.org/)
[![Status](https://img.shields.io/badge/Status-Prototype-orange.svg)](https://github.com/)
[![Hardware](https://img.shields.io/badge/Target-ESP32-black.svg)](https://www.espressif.com/en/products/socs/esp32)

## Table of Contents

- [Project Overview](#project-overview)
- [Features](#features)
- [Hardware](#hardware)
- [Requirements](#requirements)
- [Installation](#installation)
- [Configuration](#configuration)
- [Usage](#usage)
- [Directory Structure](#directory-structure)
- [State Machine](#state-machine)
- [Hardware Testing](#hardware-testing)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)
- [Acknowledgments](#acknowledgments)

## Project Overview

Power Outage Predictor is designed to monitor electrical quality in residential and small commercial environments. It continuously measures AC voltage, detects abnormal drops before a complete blackout occurs, and sends real-time alerts through SMS and MQTT so the system can respond faster and reduce downtime.

The design combines a ZMPT101B voltage sensor, ESP32 control logic, RTC timestamping, SD card logging, and local visual/audio alerts into a practical monitoring solution for power reliability.

## Features

- Real-time AC voltage monitoring using the ZMPT101B sensor
- Early warning before a complete power outage occurs
- Structured system states: NORMAL, WARNING, OUTAGE, and RESTORED
- Local alerting using LEDs and a buzzer
- Remote notifications through SMS and MQTT
- Historical data logging to a microSD card with RTC timestamps
- Built-in hardware validation mode for testing individual components

## Hardware

### Main components

- ESP32 development board
- ZMPT101B AC voltage sensor
- SIM800L GSM module
- DS3231 RTC module
- microSD card module
- Green, yellow, and red LEDs
- Active buzzer
- Resistors, jumper wires, and supporting circuitry

### Pin configuration

```text
ESP32 Pin    | Component       | Function
-------------|-----------------|------------------
GPIO32       | ZMPT101B        | ADC input
GPIO16/17    | SIM800L         | UART RX/TX
GPIO5        | SD Card         | CS
GPIO18/19/23 | SD Card         | SCK/MISO/MOSI
GPIO21/22    | DS3231 RTC      | SDA/SCL
GPIO13/14/27 | LED             | Green/Yellow/Red
GPIO4        | Buzzer          | Digital output
```

## Requirements

- PlatformIO Core or PlatformIO IDE
- Git
- A compatible ESP32 development board

## Installation

1. Clone the repository.

```bash
git clone https://github.com/username/power-outage-predictor.git
cd power-outage-predictor
```

2. Install the required libraries.

```bash
pio lib install
```

3. Build the project.

```bash
pio run
```

4. Upload the firmware to the ESP32.

```bash
pio run --target upload
```

5. Monitor the serial output.

```bash
pio device monitor
```

## Configuration

### Hardware configuration

File: `include/config.h`

```cpp
#define VOLTAGE_NORMAL_MIN 200.0
#define VOLTAGE_WARNING_DROP 20.0
#define VOLTAGE_OUTAGE_MAX 10.0
#define ZMPT101B_CALIBRATION 500.0
```

### Credentials configuration

File: `include/secrets.h`

```cpp
#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"

#define MQTT_SERVER "your-mqtt-broker.com"
#define MQTT_PORT 1883
#define MQTT_USERNAME "mqtt_user"
#define MQTT_PASSWORD "mqtt_pass"

#define SMS_TARGET_NUMBER "+6281234567890"
#define APN_NAME "internet"
#define APN_USERNAME ""
#define APN_PASSWORD ""
```

### Sensor calibration

1. Connect the ZMPT101B sensor to a known AC source.
2. Read the serial output to inspect the measured voltage.
3. Adjust `ZMPT101B_CALIBRATION` until the measured value matches the reference instrument used for testing.

## Usage

### Normal operation

1. Power on the device.
2. The green LED indicates the NORMAL state.
3. The system continuously monitors voltage.
4. Data is logged to the SD card at regular intervals.

### Event detection

- WARNING: a significant voltage drop is detected
- OUTAGE: voltage falls below the outage threshold
- RESTORED: the power supply returns to normal

### Remote monitoring

- MQTT topics used: `power-monitor/telemetry`, `power-monitor/events`, and `power-monitor/status`
- SMS alerts are sent automatically when an outage occurs and when service is restored

## Directory Structure

```text
power-outage-predictor/
├── include/
│   ├── config.h
│   ├── secrets.h
│   ├── state_machine.h
│   ├── sim800l_handler.h
│   ├── mqtt_handler.h
│   └── sd_logger.h
├── src/
│   ├── main.cpp
│   ├── state_machine.cpp
│   ├── sim800l_handler.cpp
│   ├── mqtt_handler.cpp
│   ├── sd_logger.cpp
│   └── hw_tests.h
├── platformio.ini
├── LICENSE
├── README.md
├── test/
│   └── test_main.cpp
└── .gitignore
```

## State Machine

```cpp
enum SystemState {
    STATE_NORMAL,
    STATE_WARNING,
    STATE_OUTAGE,
    STATE_RESTORED
};
```

## Hardware Testing

Testing mode can be enabled through macros defined in `src/hw_tests.h`.

```cpp
// #define TEST_LEDS
// #define TEST_BUZZER
// #define TEST_ZMPT101B
// #define TEST_SD_CARD
// #define TEST_SIM800L
```

When a test macro is enabled, the device executes the corresponding validation routine before continuing normal operation.

## Contributing

1. Fork this repository.
2. Create a feature branch for the change you want to make.
3. Make the required update and add tests where appropriate.
4. Commit your changes.
5. Open a pull request.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for full details.

## Contact

Developer: Hamin Baek

- GitHub: [@hamin-baek](https://github.com/hamin-baek)

## Acknowledgments

- ZMPT101B Arduino Library
- TinyGSM Library
- ArduinoJson
- PubSubClient
- RTClib

---

This project is well suited for residential monitoring, small facility management, and environments where early outage detection is important.
