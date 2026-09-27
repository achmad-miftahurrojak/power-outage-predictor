<div align="center">

# Power Outage Predictor

<a href="README.md"><img alt="English" src="https://img.shields.io/badge/English-DFE0E5"></a> <a href="README.id.md"><img alt="Bahasa Indonesia" src="https://img.shields.io/badge/Bahasa%20Indonesia-DFE0E5"></a> <a href="README.ko.md"><img alt="한국어" src="https://img.shields.io/badge/%ED%95%9C%EA%B5%AD%EC%96%B4-DFE0E5"></a>

<img alt="C++" src="https://img.shields.io/badge/C%2B%2B-11-00599C?logo=c%2B%2B&logoColor=white"> <img alt="ESP32" src="https://img.shields.io/badge/ESP32-E7352C?logo=espressif&logoColor=white"> <img alt="GSM" src="https://img.shields.io/badge/GSM-SIM800L-2E8B57">

ESP32 firmware that classifies AC voltage conditions and reports outages through GSM SMS.

[Features](#features) · [Architecture](ARCHITECTURE.md) · [Build](#build-and-upload) · [Project layout](#project-layout)

</div>

---

## Overview

The firmware samples an isolated AC voltage signal, calculates RMS values, moves through normal, warning, outage, and restored states, then records and reports state changes. SIM800L, SD, RTC, and MQTT support are separated so the core state machine can be tested independently.

## Features

- ZMPT101B voltage sampling and RMS calculation.
- Explicit state machine for normal, warning, outage, and restored conditions.
- Rate limited SMS alerts through SIM800L.
- SD card event logging with RTC timestamps.
- Optional MQTT reporting.
- Native tests for the state machine.

## Architecture

```text
ZMPT101B voltage sensor
              |
              v
         ESP32 ADC + RMS
              |
              v
          State machine
        /       |        \
       v        v         v
   SIM800L   SD + RTC   MQTT
      |
     SMS
```

See [ARCHITECTURE.md](ARCHITECTURE.md) for the state transitions and local configuration boundaries.

## Build and upload

### Requirements

- PlatformIO Core or PlatformIO IDE
- ESP32 development board
- ZMPT101B voltage sensor and SIM800L module
- Properly isolated and supervised mains measurement hardware

```bash
git clone https://github.com/achmad-miftahurrojak/power-outage-predictor.git
cd power-outage-predictor
pio run
pio run -t upload
```

Run the native state-machine tests with PlatformIO before connecting hardware:

```bash
pio test -e native
```

Set the serial port, alert thresholds, and local phone number in the device configuration. Keep credentials and CA material out of Git.

## Project layout

```text
src/       # Firmware and hardware adapters
include/   # State, configuration, and local secret templates
test/      # Native state-machine tests
```

## License

[MIT](LICENSE) · [GitHub profile](https://github.com/achmad-miftahurrojak)
