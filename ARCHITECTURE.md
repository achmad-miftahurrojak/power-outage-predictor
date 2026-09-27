# Architecture

```text
ZMPT101B voltage sensor
            |
            v
       ESP32 ADC + RMS
            |
            v
       State machine
       /       |       \
      v        v        v
   SIM800L   SD + RTC  MQTT (optional)
    SMS
```

The firmware samples the isolated voltage sensor, calculates RMS voltage, and transitions between normal, warning, outage, and restored states. State changes can generate a rate-limited SMS and an SD log entry. MQTT support is optional and requires a configured CA certificate.
