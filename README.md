# Power Outage Predictor

Sistem prediksi dan monitoring pemadaman listrik berbasis ESP32 untuk deteksi dini gangguan listrik dengan notifikasi real-time melalui SMS dan MQTT.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-ESP32-blue.svg)](https://platformio.org/)

## Daftar Isi

- [Fitur](#fitur)
- [Hardware](#hardware)
- [Instalasi](#instalasi)
- [Konfigurasi](#konfigurasi)
- [Cara Penggunaan](#cara-penggunaan)
- [Struktur Direktori](#struktur-direktori)
- [API Reference](#api-reference)
- [Berkontribusi](#berkontribusi)
- [Lisensi](#lisensi)
- [Kontak](#kontak)

## Fitur

- **Monitoring Real-time**: Pengukuran tegangan AC menggunakan sensor ZMPT101B
- **Prediksi Dini**: Deteksi pola penurunan tegangan sebelum pemadaman total
- **Notifikasi Multi-channel**: SMS via SIM800L dan MQTT untuk monitoring jarak jauh
- **Logging Historis**: Penyimpanan data ke SD card dengan timestamp RTC DS3231
- **State Machine**: Sistem status (NORMAL, WARNING, OUTAGE, RESTORED) dengan LED indikator
- **Alerting Akustik**: Buzzer untuk notifikasi lokal saat terjadi pemadaman
- **Hardware Testing**: Built-in test suite untuk validasi komponen hardware

## Hardware

### Komponen Utama
- **ESP32 Development Board** (ESP32-DevKitC atau sejenisnya)
- **ZMPT101B AC Voltage Sensor** - Pengukuran tegangan AC
- **SIM800L GSM Module** - Komunikasi SMS
- **DS3231 RTC Module** - Real-time clock dengan backup battery
- **MicroSD Card Module** - Data logging
- **LED Indikator** (Hijau, Kuning, Merah)
- **Buzzer Aktif** - Alert suara
- **Resistor dan kabel jumper**

### Pin Configuration
```
ESP32 Pin    | Component        | Function
-------------|------------------|------------------
GPIO32       | ZMPT101B         | ADC Input
GPIO16/17    | SIM800L          | UART RX/TX
GPIO5        | SD Card          | CS (Chip Select)
GPIO18/19/23 | SD Card          | SCK/MISO/MOSI
GPIO21/22    | DS3231 RTC       | SDA/SCL (I2C)
GPIO13/14/27 | LEDs             | Green/Yellow/Red
GPIO4        | Buzzer           | Digital Output
```

## Instalasi

### Prasyarat
- [PlatformIO Core](https://platformio.org/install) atau PlatformIO IDE
- [Git](https://git-scm.com/) untuk version control

### Langkah-langkah

1. **Clone Repository**
   ```bash
   git clone https://github.com/username/power-outage-predictor.git
   cd power-outage-predictor
   ```

2. **Install Dependencies**
   ```bash
   pio lib install
   ```

3. **Build Project**
   ```bash
   pio run
   ```

4. **Upload to ESP32**
   ```bash
   pio run --target upload
   ```

5. **Monitor Serial Output**
   ```bash
   pio device monitor
   ```

## Konfigurasi

### 1. Konfigurasi Hardware (`include/config.h`)
```cpp
// Threshold tegangan
#define VOLTAGE_NORMAL_MIN 200.0     // Tegangan minimum normal (V)
#define VOLTAGE_WARNING_DROP 20.0    // Persentase drop untuk warning (%)
#define VOLTAGE_OUTAGE_MAX 10.0      // Tegangan maksimum outage (V)

// Kalibrasi sensor
#define ZMPT101B_CALIBRATION 500.0   // Sesuaikan berdasarkan kalibrasi
```

### 2. Konfigurasi Credentials (`include/secrets.h`)
```cpp
// WiFi Configuration
#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"

// MQTT Configuration
#define MQTT_SERVER "your-mqtt-broker.com"
#define MQTT_PORT 1883
#define MQTT_USERNAME "mqtt_user"
#define MQTT_PASSWORD "mqtt_pass"

// SMS Configuration
#define SMS_TARGET_NUMBER "+6281234567890"
#define APN_NAME "internet"
#define APN_USERNAME ""
#define APN_PASSWORD ""
```

### 3. Kalibrasi Sensor ZMPT101B
1. Hubungkan sensor ke sumber AC yang diketahui tegangannya
2. Monitor output serial untuk melihat pembacaan raw
3. Sesuaikan nilai `ZMPT101B_CALIBRATION` hingga pembacaan sesuai dengan multimeter

## Cara Penggunaan

### Normal Operation
1. Power on sistem
2. LED hijau menyala menandakan status NORMAL
3. Sistem akan melakukan monitoring tegangan setiap detik
4. Data disimpan ke SD card setiap 30 detik (mode normal)

### Event Detection
- **WARNING**: LED kuning menyala saat deteksi penurunan tegangan >20%
- **OUTAGE**: LED merah + buzzer aktif saat tegangan <10V
- **RESTORED**: Kembali ke LED hijau saat tegangan normal kembali

### Monitoring Jarak Jauh
- Subscribe ke MQTT topic: `power-monitor/telemetry` dan `power-monitor/events`
- Terima SMS alert otomatis saat outage dan restored

## Struktur Direktori

```
power-outage-predictor/
├── include/                 # Header files
│   ├── config.h            # Hardware & system configuration
│   ├── secrets.h           # WiFi, MQTT, SMS credentials
│   ├── state_machine.h     # State machine definitions
│   ├── sim800l_handler.h   # GSM/SMS functions
│   ├── mqtt_handler.h      # MQTT communication
│   └── sd_logger.h         # SD card logging
├── src/                    # Source files
│   ├── main.cpp            # Main program loop
│   ├── state_machine.cpp   # State logic implementation
│   ├── sim800l_handler.cpp # GSM module handling
│   ├── mqtt_handler.cpp    # MQTT client implementation
│   ├── sd_logger.cpp       # SD card operations
│   └── hw_tests.h          # Hardware testing macros
├── platformio.ini          # PlatformIO configuration
└── README.md              # Documentation
```

## API Reference

### State Machine States
```cpp
enum SystemState {
    STATE_NORMAL,    // Tegangan normal (>200V)
    STATE_WARNING,   // Penurunan tegangan terdeteksi (>20% drop)
    STATE_OUTAGE,    // Pemadaman (<10V)
    STATE_RESTORED   // Listrik kembali normal
};
```

### MQTT Topics
- `power-monitor/telemetry` - Data tegangan berkala
- `power-monitor/events` - Event pemadaman/restored
- `power-monitor/status` - Status sistem

### Data Log Format (SD Card)
```
timestamp,voltage,current,state,event_type
2024-01-15 10:30:25,220.5,0.0,NORMAL,telemetry
2024-01-15 10:35:12,180.2,0.0,WARNING,pre_failure_pattern
2024-01-15 10:35:45,5.1,0.0,OUTAGE,outage_start
```

## Hardware Testing

Sistem menyediakan built-in hardware tests melalui macro definitions di `src/hw_tests.h`:

```cpp
// Uncomment untuk enable test tertentu
// #define TEST_LEDS
// #define TEST_BUZZER
// #define TEST_ZMPT101B
// #define TEST_SD_CARD
// #define TEST_SIM800L
```

Saat test macro aktif, sistem akan menjalankan test tersebut saat startup dan tidak melanjutkan ke operasi normal.

## Berkontribusi

1. Fork repository ini
2. Buat branch fitur (`git checkout -b fitur-baru`)
3. Commit perubahan (`git commit -am 'Tambah fitur baru'`)
4. Push ke branch (`git push origin fitur-baru`)
5. Buat Pull Request

### Guidelines
- Gunakan konvensi penamaan yang konsisten
- Tambahkan komentar untuk kode yang kompleks
- Test hardware sebelum submit perubahan
- Update dokumentasi jika diperlukan

## Lisensi

Project ini menggunakan lisensi MIT. Lihat file [LICENSE](LICENSE) untuk detail lengkap.

## Kontak

**Developer**: Hamin Baek
- Email: hamin@example.com
- GitHub: [@hamin-baek](https://github.com/hamin-baek)

### Acknowledgments
- [ZMPT101B Arduino Library](https://github.com/Abdurraziq/ZMPT101B-arduino) oleh Abdurraziq
- [TinyGSM Library](https://github.com/vshymanskyy/TinyGSM) untuk komunikasi GSM
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) untuk serialisasi data
- [PubSubClient](https://github.com/knolleary/PubSubClient) untuk MQTT client
- [RTClib](https://github.com/adafruit/RTClib) untuk RTC DS3231 support

---

**Catatan**: Sistem ini dirancang untuk monitoring residensial/komersial kecil. Untuk aplikasi industri atau critical infrastructure, diperlukan additional safety measures dan redundancy.