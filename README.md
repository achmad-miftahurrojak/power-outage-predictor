<div align="center">

# Power Outage Predictor

ESP32 firmware that classifies AC voltage conditions and reports outages through GSM SMS.

![C++](https://img.shields.io/badge/C%2B%2B-11-00599C?logo=c%2B%2B&logoColor=white) ![PlatformIO](https://img.shields.io/badge/PlatformIO-Core-F56600?logo=platformio&logoColor=white) ![ESP32](https://img.shields.io/badge/ESP32-Espressif-E7352C) ![GSM](https://img.shields.io/badge/GSM-SIM800L-2E8B57)

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
