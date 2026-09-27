<div align="center">

# Power Outage Predictor

[English](README.md) · [Bahasa Indonesia](README.id.md) · [한국어](README.ko.md)

AC 전압 상태를 분류하고 GSM SMS로 정전을 보고하는 ESP32 펌웨어입니다.

</div>

---

## 개요

절연된 AC 전압 신호를 샘플링하고 RMS를 계산한 뒤 normal, warning, outage, restored 상태를 관리합니다. 상태 변경은 기록되고 SIM800L을 통해 전송될 수 있습니다.

## 기능

- ZMPT101B 전압 샘플링 및 RMS 계산
- 상태별 state machine
- SIM800L SMS 경보
- RTC timestamp를 포함한 SD 기록
- 선택 가능한 MQTT 보고
- state machine native test

## 아키텍처

상태 전환과 로컬 설정 경계는 [ARCHITECTURE.md](ARCHITECTURE.md)에서 확인할 수 있습니다.

## 빌드 및 업로드

필요 항목: PlatformIO, ESP32 보드, ZMPT101B 센서, SIM800L 모듈.

    git clone https://github.com/achmad-miftahurrojak/power-outage-predictor.git
    cd power-outage-predictor
    pio run
    pio run -t upload
    pio test -e native

시리얼 포트, 경보 threshold, 전화번호를 설정하세요. 인증 정보와 CA material은 Git에 올리지 않습니다.

## 프로젝트 구조

    src/       # 펌웨어 및 하드웨어 adapter
    include/   # 상태, 설정, 로컬 secret template
    test/      # native state-machine test

## 라이선스

[MIT](LICENSE) · [GitHub 프로필](https://github.com/achmad-miftahurrojak)
