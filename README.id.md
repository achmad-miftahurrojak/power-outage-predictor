<div align="center">

# Power Outage Predictor

<a href="README.md"><img alt="English" src="https://img.shields.io/badge/English-DFE0E5"></a> <a href="README.id.md"><img alt="Bahasa Indonesia" src="https://img.shields.io/badge/Bahasa%20Indonesia-DFE0E5"></a> <a href="README.ko.md"><img alt="한국어" src="https://img.shields.io/badge/%ED%95%9C%EA%B5%AD%EC%96%B4-DFE0E5"></a>

<img alt="C++" src="https://img.shields.io/badge/C%2B%2B-11-00599C?logo=c%2B%2B&logoColor=white"> <img alt="ESP32" src="https://img.shields.io/badge/ESP32-E7352C?logo=espressif&logoColor=white"> <img alt="GSM" src="https://img.shields.io/badge/GSM-SIM800L-2E8B57">

Firmware ESP32 yang mengklasifikasikan kondisi tegangan AC dan mengirim laporan padam listrik melalui SMS GSM.

</div>

---

## Ringkasan

Firmware mengambil sampel sinyal tegangan AC terisolasi, menghitung RMS, berpindah di antara status normal, warning, outage, dan restored, lalu mencatat serta melaporkan perubahan status.

## Fitur

- Sampling tegangan ZMPT101B dan perhitungan RMS.
- State machine untuk kondisi normal, warning, outage, dan restored.
- Alert SMS dengan SIM800L.
- Logging kejadian ke SD card dengan RTC.
- Pelaporan MQTT opsional.
- Native test untuk state machine.

## Arsitektur

Lihat [ARCHITECTURE.md](ARCHITECTURE.md) untuk alur state dan batas konfigurasi lokal.

## Build dan upload

Persyaratan: PlatformIO, board ESP32, sensor ZMPT101B, dan modul SIM800L.

    git clone https://github.com/achmad-miftahurrojak/power-outage-predictor.git
    cd power-outage-predictor
    pio run
    pio run -t upload
    pio test -e native

Atur port serial, threshold alert, dan nomor telepon lokal. Jangan commit kredensial atau material CA.

## Struktur proyek

    src/       # Firmware dan adapter hardware
    include/   # State, konfigurasi, dan template secret lokal
    test/      # Native state-machine tests

## Lisensi

[MIT](LICENSE) · [Profil GitHub](https://github.com/achmad-miftahurrojak)
