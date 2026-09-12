# Power Outage Predictor

An early warning system that monitors AC voltage patterns to detect severe brownouts and predict power grid failures.

![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)
![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32-lightgrey.svg)
![Language: C++](https://img.shields.io/badge/Language-C++-red.svg)

## Table of Contents
1. [Features](#features)
2. [Hardware Requirements](#hardware-requirements)
3. [Getting Started](#getting-started)
4. [Usage](#usage)
5. [Directory Structure](#directory-structure)
6. [Contributing](#contributing)
7. [License](#license)
8. [Contact](#contact)

## Features
* Accurate AC voltage Root Mean Square calculation using a ZMPT101B sensor.
* Pattern recognition to confirm critical voltage drops before issuing alerts.
* Cellular integration using a SIM800L GSM module for SMS notifications.
* System fault tolerance utilizing the ESP32 Hardware Watchdog Timer.

## Hardware Requirements
* ESP32 Development Board
* ZMPT101B AC Voltage Sensor
* SIM800L GSM Module

## Getting Started

### Prerequisites
* PlatformIO IDE.
* An active SIM card with SMS capabilities.

### Installation
```bash
git clone https://github.com/hamin-baek/power-outage-predictor.git
cd power-outage-predictor
pio run --target upload
```

## Usage
Connect the ZMPT101B to the mains voltage line safely and connect the GSM module to the serial pins defined in the source code. The device will monitor the voltage and send an SMS alert if the voltage drops below the defined threshold for a sustained period.

## Directory Structure
```text
src/
  main.cpp          # Main firmware logic
include/            # Header files
lib/                # Project specific libraries
platformio.ini      # Build configuration
```

## Contributing
Please open an issue first to discuss any proposed changes before submitting a pull request.

## License
This project is licensed under the MIT License.

## Contact
Developed by Achmad Miftahurrojak.
GitHub: [hamin-baek](https://github.com/hamin-baek)

***
**Description:** Predictive power grid failure detection system using GSM alerts.
**Tags:** esp32, power-grid, gsm
