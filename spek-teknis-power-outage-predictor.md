# Spesifikasi Teknis — Mati Listrik Predictor + Alert (Proyek 2)

**Konfigurasi:** Mains-powered (adaptor 5V dari PLN) + baterai backup untuk deteksi durasi mati listrik · Dual connectivity: SIM800L (SMS, jalur utama alert) + WiFi/MQTT (jalur data rutin ke dashboard)

---

## 1. Device Requirements Document (DRD)

### 1.1 Sensor & Aktuator

| Komponen | Tipe | Fungsi |
|---|---|---|
| Sensor tegangan ZMPT101B | Analog (AC voltage transformer) | Ukur RMS tegangan AC PLN secara real-time |
| Sensor arus ACS712 (opsional) | Analog | Monitor beban listrik, berguna untuk deteksi lonjakan/anomali selain drop tegangan |
| Modul SIM800L + antena | Aktuator komunikasi | Kirim SMS alert — jalur utama, independen dari WiFi/listrik lokal |
| Buzzer aktif | Aktuator | Alert suara lokal saat listrik mati/drop |
| LED indikator (hijau=normal, kuning=warning, merah=mati) | Aktuator | Indikator visual cepat |
| Modul SD card | Storage lokal | Log riwayat mati listrik (untuk lapor ke PLN) |
| RTC DS3231 | Timekeeping | Timestamp akurat kapan mulai/selesai mati, independen dari NTP |

### 1.2 Sampling Rate

Beda karakter dari Proyek 1 — di sini **frekuensi tinggi justru penting** karena tujuannya deteksi *pre-failure pattern* (drop tegangan sesaat sebelum mati total), bukan tren lambat seperti kualitas air.

- **Baca ADC tegangan: kontinu, ambil banyak sample per detik** (misal 1000-2000 sample/detik) untuk hitung **RMS** dalam window ~1 detik — tegangan AC berosilasi tiap siklus (20ms di 50Hz), jadi single-point read tidak representatif.
- **Evaluasi RMS hasil hitung: setiap 1 detik.**
- **Publish telemetri rutin ke cloud: tiap 30-60 detik** (kirim ringkasan avg/min/max dari window itu, bukan tiap detik) — supaya tidak membanjiri MQTT broker dengan data yang sebagian besar redundan saat kondisi normal.
- **Event (WARNING/OUTAGE/RESTORED): kirim SEGERA saat terjadi**, tidak menunggu jadwal 30-60 detik — ini yang paling kritis dan harus event-driven murni.

### 1.3 Real-time vs Batch

**Beda signifikan dari Proyek 1 — di sini lebih condong ke near-real-time, bukan batch:**

- **No deep sleep.** Device ini mains-powered (bukan baterai primer), jadi tidak ada constraint daya yang memaksa sleep cycle seperti Proyek 1. Monitoring harus kontinu karena dip tegangan sesaat (indikator pre-failure) bisa terlewat kalau device sempat tidur.
- **Alert (WARNING/OUTAGE/RESTORED): real-time, event-driven** — begitu kondisi terdeteksi, langsung trigger SMS + buzzer + publish MQTT, tanpa delay.
- **Data telemetri rutin (tegangan normal): batch tiap 30-60 detik** — ini bagian yang boleh "batch" karena data tegangan normal tidak perlu granularitas tinggi untuk dashboard, cukup untuk keperluan tren.
- **Log ke SD card: rate adaptif** — saat kondisi NORMAL, log ringkasan tiap 30 detik cukup; saat kondisi WARNING/OUTAGE, naikkan ke log tiap 1 detik untuk dapat detail lengkap kejadian (berguna untuk data yang dilaporkan ke PLN).

---

## 2. Bill of Materials (BOM)

| Komponen | Fungsi | Estimasi Harga |
|---|---|---|
| ESP32 DevKit V1 | Otak sistem | Rp50.000 |
| Sensor tegangan ZMPT101B | Baca RMS tegangan AC | Rp40.000–70.000 |
| Sensor arus ACS712 (20A/30A, opsional) | Monitor beban | Rp25.000–40.000 |
| Modul SIM800L + antena | SMS alert, jalur utama | Rp60.000–90.000 |
| Kartu SIM prabayar | Komunikasi SMS | ~Rp20.000/bulan (isi ulang, biaya operasional bukan one-time) |
| Buzzer aktif | Alert suara | Rp5.000 |
| LED merah/kuning/hijau + resistor | Indikator visual | Rp5.000 |
| Modul Micro SD + kartu 8GB | Log riwayat mati listrik | Rp30.000 |
| RTC DS3231 | Timestamp | Rp20.000 |
| Adaptor 5V 2A (dari colokan PLN) | Power utama | Rp30.000–50.000 |
| Baterai backup: Li-ion 18650 (2x) + modul charge/boost (TP4056 + step-up ke 5V) | Supaya ESP32+SIM800L tetap hidup saat listrik mati, untuk deteksi durasi & kirim SMS terakhir | Rp70.000–110.000 |
| Kapasitor besar (1000-2200µF, low-ESR) dekat SIM800L | Buffer arus untuk spike transmisi SIM800L (lihat catatan Power Budget) | Rp5.000–10.000 |
| Box enclosure + terminal block + isolasi kabel sisi AC | Proteksi + keselamatan wiring listrik | Rp60.000–100.000 |

**Total estimasi: Rp400.000–530.000 one-time** + ~Rp20.000/bulan pulsa SMS.

⚠️ **Catatan keselamatan wiring:** ZMPT101B menyentuh tegangan AC PLN 220V. Sisi yang terhubung ke jalur AC (bukan sisi output sensor ke ESP32) **wajib dikerjakan hati-hati atau dengan bantuan orang yang paham instalasi listrik** — modul ZMPT101B yang dijual umum sudah punya isolasi built-in via transformer, tapi tetap jangan bongkar/sambung dalam keadaan menyala, dan pastikan enclosure menutup rapat bagian bertegangan AC.

---

## 3. System Architecture (3 Layer)

```
                     ┌─────────────────────────────┐
                     │        DEVICE LAYER           │
                     │  ESP32 + ZMPT101B + ACS712    │
                     │  + Buzzer/LED + SD + RTC       │
                     │  + baterai backup + kapasitor  │
                     └───────────┬───────────┬───────┘
                                 │           │
                    SMS (GSM,    │           │  WiFi (MQTT/TLS)
                    independen    │           │  — jalur sekunder,
                    dari listrik  │           │  bisa ikut mati
                    lokal)        │           │  bareng listrik
                                 ▼           ▼
                     ┌──────────────┐  ┌──────────────────────┐
                     │  KEPALA DESA   │  │   CLOUD/BACKEND LAYER  │
                     │  (HP langsung  │  │  MQTT Broker (TLS)     │
                     │  terima SMS)   │  │  → Backend/Processor   │
                     └──────────────┘  │  → Database (time-series)│
                                        │  → Dashboard web         │
                                        │  → Riwayat untuk lapor PLN│
                                        └──────────────────────────┘
```

**Ini yang membedakan arsitektur Proyek 2 dari Proyek 1:** ada **dua jalur output paralel**, bukan satu. SMS langsung ke HP kepala desa itu **tidak lewat cloud sama sekali** — sengaja didesain begitu supaya alert paling kritis (mati listrik) tidak bergantung pada infrastruktur cloud yang notabene juga butuh internet (yang sama rentannya dengan WiFi lokal). Cloud/dashboard tetap berguna untuk data historis & lapor PLN, tapi bukan jalur satu-satunya untuk notifikasi darurat.

**Device layer** di sini juga berperan sebagai edge (sama seperti Proyek 1) — keputusan "ini kondisi WARNING/OUTAGE" dievaluasi lokal di ESP32, tidak menunggu konfirmasi dari cloud.

**Cloud stack:** lanjutkan pakai Adafruit IO (kalau masih pilot) atau migrasi ke Mosquitto+Node-RED+InfluxDB+Grafana kalau sudah mulai gabung dengan Proyek 1 di infrastruktur yang sama — ini poin bagus untuk **reuse infrastruktur cloud yang sama antar proyek**, tidak perlu setup baru per proyek.

---

## 4. Connectivity & Protocol Plan

### 4.1 Dua jalur komunikasi (ini perbedaan utama dari Proyek 1)

| Jalur | Fungsi | Kenapa |
|---|---|---|
| **SIM800L → SMS** | Alert kritis (WARNING/OUTAGE/RESTORED) langsung ke HP kepala desa | Independen dari listrik & WiFi lokal — tower GSM umumnya punya backup power sendiri, jadi tetap jalan saat listrik desa padam |
| **WiFi → MQTT (TLS)** | Data telemetri rutin + histori untuk dashboard & lapor PLN | Lebih kaya data & murah (tidak kena biaya SMS), tapi **tidak reliable saat listrik mati** — dipakai untuk data non-kritis saja |

**Prinsip desain:** SMS adalah jalur wajib untuk apapun yang sifatnya alert darurat. MQTT/WiFi adalah "bonus" untuk visibility dashboard, bukan pengganti SMS. Kalau WiFi kebetulan mati bareng listrik (skenario paling umum), sistem tetap berfungsi penuh untuk fungsi intinya lewat SMS.

### 4.2 Protokol MQTT (untuk jalur WiFi)
- Sama seperti Proyek 1: **MQTT over TLS, port 8883, QoS 1**.
- Topic structure:
  - `desa/{nama_desa}/power/{device_id}/telemetry` — data tegangan rutin (batch 30-60s)
  - `desa/{nama_desa}/power/{device_id}/event` — event WARNING/OUTAGE/RESTORED
  - `desa/{nama_desa}/power/{device_id}/status` — online/offline (LWT)

### 4.3 SIM800L (AT command, bukan MQTT)
- SIM800L komunikasi ke ESP32 lewat **UART + AT command** (library `TinyGSM` menyederhanakan ini).
- Kirim SMS langsung ke nomor HP kepala desa (bisa lebih dari 1 nomor untuk redundansi — misal kepala desa + operator PLN setempat).
- **Tidak perlu koneksi persistent** — SIM800L cukup aktif saat perlu kirim SMS, bisa dimatikan/idle di antaranya untuk kurangi beban arus (meski di proyek ini bukan constraint utama karena mains-powered).

### 4.4 Koneksi WiFi — behaviour beda dari Proyek 1
- Karena mains-powered (bukan baterai), **boleh persistent connection** (tidak perlu connect-publish-disconnect cycle seperti Proyek 1) — device bisa tetap terhubung MQTT terus-menerus untuk publish real-time.
- Tetap pakai reconnect logic dengan backoff kalau WiFi putus, tapi tidak perlu seagresif menghemat daya seperti Proyek 1.

---

## 5. Data Schema

### 5.1 Payload MQTT — telemetri rutin (topik `telemetry`, tiap 30-60s)

```json
{
  "ts": 1735459200,
  "v_avg": 218.4,
  "v_min": 215.1,
  "v_max": 221.0,
  "i_avg": 2.3
}
```

### 5.2 Payload MQTT — event (topik `event`, segera saat terjadi)

```json
{
  "ts": 1735459260,
  "type": "outage_start",
  "v_before_drop": 178.2
}
```

Tipe event: `warning` (pre-failure pattern terdeteksi), `outage_start`, `outage_end` (dengan field tambahan `duration_sec`), `restored`.

### 5.3 Isi SMS (ringkas, karena SMS punya limit karakter & biaya per SMS)

```
[ALERT] Listrik padam pukul 14:21. Tegangan sebelum drop: 178V.
```
```
[INFO] Listrik nyala kembali pukul 15:03. Durasi padam: 42 menit.
```

Format singkat dan langsung actionable — hindari jargon teknis di SMS karena penerimanya (kepala desa) belum tentu paham istilah IoT.

### 5.4 Skema log SD card (rate adaptif sesuai Bagian 1.3)

```
timestamp,v_rms,i_rms,state,event_type
2026-08-31T14:20:30,219.1,2.1,NORMAL,
2026-08-31T14:20:59,178.2,2.4,WARNING,pre_failure_pattern
2026-08-31T14:21:00,0.0,0.0,OUTAGE,outage_start
2026-08-31T15:03:12,220.5,2.0,RESTORED,outage_end
```

---

## 6. Power Budget

Karakter power budget di proyek ini **beda total dari Proyek 1** — bukan soal hemat baterai untuk umur pakai panjang, tapi soal **memastikan device tetap hidup cukup lama saat listrik utama mati** untuk mendeteksi & melaporkan kejadian itu.

### 6.1 Kondisi normal (listrik PLN menyala)
- Semua komponen disuplai dari adaptor 5V 2A — tidak ada constraint daya berarti, ESP32+ZMPT101B+SD+RTC+WiFi bisa jalan kontinu tanpa masalah (total konsumsi biasanya di bawah 500mA, adaptor 2A punya banyak margin).

### 6.2 Kondisi mati listrik (baterai backup aktif) — ini yang perlu dihitung cermat
Saat adaptor PLN mati, sistem harus switch otomatis ke baterai backup (perlu rangkaian auto-switch, misal pakai diode OR-ing atau modul UPS kecil sederhana) untuk:
1. Tetap hidup cukup lama untuk mencatat waktu mulai mati (RTC tetap jalan)
2. **Kirim SMS via SIM800L** — ini yang paling boros arus
3. Idealnya tetap monitoring sampai listrik nyala lagi (untuk hitung durasi), atau minimal cukup lama untuk kirim SMS awal lalu masuk mode hemat daya

**⚠️ Catatan kritis soal SIM800L:** modul ini punya **spike arus hingga ~2A** sesaat saat transmit (bukan konsumsi rata-rata biasa yang cuma puluhan-ratusan mA). Ini penyebab #1 kegagalan proyek SIM800L pemula — kalau power supply/baterai tidak sanggup suplai spike ini, SIM800L bisa restart sendiri atau gagal kirim SMS tepat saat paling dibutuhkan. **Solusi:** kapasitor besar (1000-2200µF low-ESR) dipasang sedekat mungkin ke pin power SIM800L untuk buffer spike ini, dan pastikan baterai/boost converter backup punya rating arus cukup (bukan cuma kapasitas mAh, tapi juga *discharge rate*-nya).

### 6.3 Estimasi kebutuhan baterai backup

| Skenario | Estimasi arus | Durasi dibutuhkan |
|---|---|---|
| ESP32 + ZMPT101B + RTC (monitoring standby) | ~100-150 mA | Selama listrik mati (bisa jam-an) |
| SIM800L saat transmit SMS (beberapa kali: warning + outage + restored) | spike ~2A selama beberapa detik per SMS, rata-rata ~200mA saat registered ke network | Beberapa kali kejadian singkat |
| Buzzer aktif | ~20-30mA | Saat alert aktif |

**Rekomendasi:** baterai 2x 18650 (total ~4000-6000mAh) dengan boost converter yang sanggup suplai minimal 2A burst — ini kasih margin untuk device tetap hidup **beberapa jam** saat mati listrik (mati listrik di desa kadang tidak sebentar), cukup untuk kirim SMS awal, tetap logging, dan kirim SMS "restored" begitu listrik nyala lagi.

---

## 7. Firmware Architecture

### 7.1 State machine (berbeda dari Proyek 1 — kontinu, bukan sleep/wake cycle)

```
[BOOT] → [INIT: ESP32, ZMPT101B, SIM800L registrasi network, RTC, SD, WiFi/MQTT connect]
   │
   ▼
[LOOP KONTINU — tidak ada deep sleep]
   │
   ├─▶ [Sample ADC ZMPT101B: ~1000-2000x/detik]
   │
   ├─▶ [Hitung RMS tiap window 1 detik]
   │
   ├─▶ [EVALUASI STATE]
   │        │
   │        ├─ NORMAL (tegangan stabil >200V misal) → log ringkas tiap 30s, publish MQTT tiap 30-60s
   │        │
   │        ├─ WARNING (drop >20% dalam 5 detik, tapi belum 0) → 
   │        │     nyalakan LED kuning, publish event SEGERA,
   │        │     kirim SMS warning, naikkan rate logging ke 1s
   │        │
   │        ├─ OUTAGE (tegangan = 0) →
   │        │     switch ke baterai backup (otomatis via hardware),
   │        │     catat waktu mulai (RTC), nyalakan LED merah + buzzer,
   │        │     kirim SMS outage_start, publish MQTT (kalau WiFi masih hidup —
   │        │     biasanya tidak, tapi coba saja, fail gracefully kalau tidak)
   │        │     lanjut monitoring tegangan tiap detik menunggu listrik kembali
   │        │
   │        └─ RESTORED (tegangan kembali stabil) →
   │              hitung durasi OUTAGE, catat ke SD,
   │              kirim SMS restored + durasi,
   │              publish event ke MQTT, kembali ke state NORMAL
   │
   ▼
[ulangi loop]
```

### 7.2 Watchdog Timer
- Sama pentingnya seperti Proyek 1 — aktifkan hardware watchdog ESP32.
- **Perhatian khusus:** loop kontinu tanpa sleep berarti risiko hang lebih tinggi kalau ada bug di parsing AT command SIM800L (library GSM terkenal kadang bisa stuck menunggu response modem) — pastikan semua komunikasi dengan SIM800L punya **timeout eksplisit**, jangan `while(!response)` tanpa batas.

### 7.3 Reconnect & Retry Logic
- **WiFi/MQTT**: reconnect dengan backoff, tapi karena ini jalur sekunder (bukan kritis), boleh retry lebih santai — kegagalan WiFi tidak boleh mengganggu loop utama monitoring tegangan & SMS.
- **SIM800L**: retry kirim SMS beberapa kali kalau gagal (jaringan GSM kadang butuh beberapa percobaan), tapi tetap ada batas maksimum supaya tidak stuck di loop retry selamanya saat kondisi OUTAGE yang justru butuh CPU untuk terus monitoring tegangan.
- **Prioritas eksekusi jelas:** monitoring tegangan & evaluasi state > kirim SMS > publish MQTT. Kalau CPU/waktu terbatas saat kondisi darurat, jangan sampai proses MQTT yang gagal-gagal menghambat pengiriman SMS.

### 7.4 Error Handling
- Sensor ZMPT101B baca nilai aneh (misal karena noise elektrik) → filter dengan **hysteresis** (butuh beberapa sample konsisten sebelum ganti state, bukan langsung dari 1 sample outlier) supaya tidak false alarm tiap ada noise sesaat.
- SIM800L gagal registrasi network (sinyal GSM lemah di lokasi) → ini risiko nyata untuk lokasi dengan sinyal buruk, perlu dicek saat survey lokasi (poin baru yang perlu ditambahkan ke checklist deployment).

---

## 8. Security Plan

### 8.1 Autentikasi & Enkripsi
- Sama dengan Proyek 1: MQTT over TLS (8883), username/password unik per device, credential tidak di-hardcode ke repo public.
- **SMS tidak dienkripsi** (SMS secara protokol memang plaintext) — untuk konten SMS di proyek ini (info mati listrik) risikonya rendah, tidak perlu diproteksi khusus. Tapi pastikan **nomor tujuan SMS di-hardcode dengan validasi**, jangan ambil nomor tujuan dari input eksternal manapun (mencegah SIM800L disalahgunakan kirim SMS ke sembarang nomor kalau ada bug/exploit).

### 8.2 OTA Firmware Update
- Sama seperti Proyek 1: HTTPS-based OTA dengan verifikasi firmware sebelum flash, dual partition untuk fail-safe.
- Karena device ini mains-powered, **cek OTA boleh lebih sering** (tidak perlu sehemat Proyek 1) — tapi tetap disarankan trigger manual lewat MQTT command daripada auto-check tiap loop, supaya tidak mengganggu monitoring kontinu.

### 8.3 Checklist keamanan minimum
- [x] MQTT over TLS (jalur sekunder)
- [x] Username/password unik per device
- [x] Credential tidak di-hardcode ke repo public
- [x] Validasi nomor tujuan SMS di-hardcode, tidak dinamis dari luar
- [x] OTA dengan verifikasi firmware
- [ ] Enkripsi SMS — tidak applicable/diperlukan untuk use case ini

---

## Ringkasan Perbedaan Kunci dari Proyek 1

| Aspek | Proyek 1 (Water Quality) | Proyek 2 (Power Outage) |
|---|---|---|
| Power model | Baterai+solar, deep sleep agresif | Mains-powered + baterai backup untuk kontinuitas darurat |
| Monitoring | Periodik (tiap 10 menit) | Kontinu (real-time, no sleep) |
| Jalur alert utama | Buzzer/LED lokal + MQTT | SMS (SIM800L) — independen dari listrik/WiFi lokal |
| Risiko desain terbesar | Baterai habis sebelum solar charge | Spike arus SIM800L saat momen paling kritis (mati listrik) |
| MQTT connection style | Connect-publish-disconnect per cycle | Persistent connection (karena tidak ada constraint daya) |

Update juga dokumen monorepo kamu di bagian spesifikasi `power-outage-predictor` — tambahkan SIM800L ke tabel komponennya dan hapus open question soal mode operasi (sudah confirmed: kontinu, no deep sleep).
