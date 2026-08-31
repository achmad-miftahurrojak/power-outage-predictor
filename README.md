# Power Outage Predictor

Power Outage Predictor is an ESP32-based monitoring system for early detection of electrical disturbances and outages. It measures AC voltage, identifies warning conditions before a total blackout, and sends alerts through SMS and MQTT for faster response.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-ESP32-blue.svg)](https://platformio.org/)

## Deskripsi Proyek

Project ini dirancang untuk memantau kualitas listrik pada sistem rumah tangga atau fasilitas kecil. Dengan kombinasi sensor tegangan, pemrosesan state machine, log ke SD card, serta notifikasi melalui SMS dan MQTT, perangkat ini dapat membantu mendeteksi awal adanya penurunan tegangan dan pemadaman listrik secara lebih cepat.

Tags: esp32, iot, power-monitoring

## Fitur

- Monitoring tegangan AC secara real time menggunakan sensor ZMPT101B
- Deteksi dini penurunan tegangan sebelum pemadaman total terjadi
- Status sistem yang terstruktur: NORMAL, WARNING, OUTAGE, dan RESTORED
- Peringatan lokal melalui LED dan buzzer
- Notifikasi jarak jauh menggunakan SMS dan MQTT
- Penyimpanan data historis ke microSD card dengan timestamp RTC
- Validasi perangkat keras menggunakan mode testing yang dapat diaktifkan secara terpisah

## Hardware

### Komponen utama

- ESP32 development board
- Sensor tegangan AC ZMPT101B
- Modul GSM SIM800L
- RTC DS3231
- Modul microSD card
- LED indikator hijau, kuning, dan merah
- Buzzer aktif
- Resistor, kabel jumper, dan rangkaian pendukung

### Konfigurasi pin

```text
ESP32 Pin    | Komponen        | Fungsi
-------------|-----------------|------------------
GPIO32       | ZMPT101B        | Input ADC
GPIO16/17    | SIM800L         | UART RX/TX
GPIO5        | SD Card         | CS
GPIO18/19/23 | SD Card         | SCK/MISO/MOSI
GPIO21/22    | DS3231 RTC      | SDA/SCL
GPIO13/14/27 | LED             | Hijau/Kuning/Merah
GPIO4        | Buzzer          | Output digital
```

## Persyaratan

- PlatformIO Core atau PlatformIO IDE
- Git
- ESP32 board yang kompatibel

## Instalasi

1. Clone repository.

```bash
git clone https://github.com/username/power-outage-predictor.git
cd power-outage-predictor
```

2. Install dependency library.

```bash
pio lib install
```

3. Build proyek.

```bash
pio run
```

4. Upload ke ESP32.

```bash
pio run --target upload
```

5. Monitor output serial.

```bash
pio device monitor
```

## Konfigurasi

### Konfigurasi perangkat keras

File: `include/config.h`

```cpp
#define VOLTAGE_NORMAL_MIN 200.0
#define VOLTAGE_WARNING_DROP 20.0
#define VOLTAGE_OUTAGE_MAX 10.0
#define ZMPT101B_CALIBRATION 500.0
```

### Konfigurasi kredensial

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

### Kalibrasi sensor

1. Hubungkan sensor ZMPT101B ke sumber AC yang diketahui.
2. Baca output serial untuk melihat nilai tegangan.
3. Sesuaikan `ZMPT101B_CALIBRATION` sampai hasil pembacaan sesuai dengan alat ukur yang digunakan.

## Cara penggunaan

### Kondisi normal

1. Nyalakan perangkat.
2. LED hijau menunjukkan status NORMAL.
3. Sistem memantau tegangan setiap detik.
4. Data dicatat ke SD card secara berkala.

### Deteksi kejadian

- WARNING: terjadi penurunan tegangan yang signifikan
- OUTAGE: tegangan turun di bawah ambang pemadaman
- RESTORED: kondisi listrik kembali normal

### Monitoring jarak jauh

- MQTT topic yang digunakan: `power-monitor/telemetry`, `power-monitor/events`, dan `power-monitor/status`
- SMS akan dikirim otomatis saat terjadi outage dan saat kondisi normal kembali

## Struktur direktori

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
└── test/
    └── test_main.cpp
```

## State machine

```cpp
enum SystemState {
    STATE_NORMAL,
    STATE_WARNING,
    STATE_OUTAGE,
    STATE_RESTORED
};
```

## Pengujian perangkat keras

Mode pengujian dapat diaktifkan melalui macro di `src/hw_tests.h`.

```cpp
// #define TEST_LEDS
// #define TEST_BUZZER
// #define TEST_ZMPT101B
// #define TEST_SD_CARD
// #define TEST_SIM800L
```

Saat macro aktif, perangkat akan menjalankan pengujian tertentu sebelum masuk ke operasi normal.

## Kontribusi

1. Fork repositori ini.
2. Buat branch baru untuk fitur yang akan dikembangkan.
3. Lakukan perubahan dan tambahkan pengujian jika diperlukan.
4. Commit perubahan.
5. Buat pull request.

## Lisensi

Proyek ini dilisensikan di bawah MIT License. Lihat [LICENSE](LICENSE) untuk detail lengkap.

## Kontak

Developer: Hamin Baek

- GitHub: [@hamin-baek](https://github.com/hamin-baek)

## Ucapan terima kasih

- ZMPT101B Arduino Library
- TinyGSM Library
- ArduinoJson
- PubSubClient
- RTClib

---

Project ini cocok digunakan untuk monitoring listrik rumah tangga, fasilitas kecil, atau lingkungan yang memerlukan deteksi dini pemadaman listrik.
