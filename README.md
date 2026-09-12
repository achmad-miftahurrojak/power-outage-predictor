# Power Outage Predictor

An intelligent hardware monitoring system designed to detect and predict electrical grid failures using precision AC voltage analysis.

![C++](https://img.shields.io/badge/C++-11-00599C?logo=c%2B%2B&logoColor=white)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Core-F56600?logo=platformio&logoColor=white)
![ESP32](https://img.shields.io/badge/ESP32-Espressif-E7352C)

## Table of Contents

1. [Features](#features)
2. [Screenshot](#screenshot)
3. [Getting Started](#getting-started)
4. [Usage](#usage)
5. [Directory Structure](#directory-structure)
6. [API Reference](#api-reference)
7. [Contributing](#contributing)
8. [License](#license)
9. [Contact](#contact)

## Features

- Precision Sensing: Utilizes the ZMPT101B module for accurate AC voltage waveform analysis.
- Instantaneous Alerts: Integrated SIM800L module for immediate SMS dispatch upon grid failure detection.
- Fail-Safe Operation: Designed to operate on backup power to ensure alert delivery during an active outage.
- Optimized Build System: Leverages Turborepo and PlatformIO for rapid, reproducible firmware compilation.

## Screenshot

![Hardware Prototype](https://via.placeholder.com/800x450?text=Hardware+Prototype+Demo)

## Getting Started

### Prerequisites

- PlatformIO IDE or CLI
- Node.js (for Turborepo orchestration)
- Hardware: ESP32 development board, ZMPT101B voltage sensor, SIM800L GSM module

### Installation Steps

```bash
git clone https://github.com/hamin-baek/hamin-baek.git
cd hardware/power-outage-predictor
npm install
```

### Configuration

Modify the `platformio.ini` file to match the COM port of your connected ESP32 device. Adjust alert thresholds and target phone numbers within the source code configuration headers.

## Usage

Build the firmware:
```bash
npm run build
```

Upload the firmware to the device:
```bash
npm run upload
```

## Directory Structure

- `src/`: Core application logic and sensor reading algorithms.
- `include/`: Header files and system configuration definitions.
- `test/`: Hardware unit tests for sensor calibration verification.

## API Reference

This system operates autonomously. Internal module interfaces (e.g., ZMPT101B initialization, SIM800L AT command sequences) are documented directly within the `src/` directory files.

## Contributing

Ensure that modifications to the voltage sampling routines do not introduce blocking delays that could interfere with the GSM module communication loop.

## License

This project is licensed under the MIT License.

## Contact

Created by Achmad Miftahurrojak.
[GitHub](https://github.com/hamin-baek)
