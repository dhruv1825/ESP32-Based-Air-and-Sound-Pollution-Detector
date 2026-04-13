# ESP32-Based Air and Sound Pollution Detector

> Real-time environmental monitoring using ESP32 — tracks air quality (MQ2 gas sensor) and sound levels (I2S microphone) with a live web dashboard.

---

## 📸 Overview

The ESP32-Based Air and Sound Pollution Detector is a full-stack IoT project that streams sensor data from an ESP32 to a browser-based dashboard over USB Serial (Web Serial API). No server, no cloud — just plug in and monitor.

| Metric | Sensor | Range | Unit |
|--------|--------|-------|------|
| Air Quality | MQ2 Gas Sensor | 0 – 3000 | µg/m³ |
| Sound Level | I2S Microphone (INMP441) | 0 – 120 | dB |

---

## 🛠 Hardware Requirements

| Component | Details |
|-----------|---------|
| ESP32 | DOIT DevKit V1 (or compatible) |
| MQ2 Gas Sensor | Analog output → GPIO34 |
| I2S Microphone | INMP441 or compatible |
| USB Cable | For power + serial communication |

### Wiring

**MQ2 Gas Sensor**
```
MQ2 AOUT → GPIO34 (ADC1_CH6)
MQ2 VCC  → 3.3V or 5V
MQ2 GND  → GND
```

**I2S Microphone**
```
MIC WS   → GPIO25
MIC SD   → GPIO33
MIC SCK  → GPIO26
MIC VDD  → 3.3V
MIC GND  → GND
MIC L/R  → GND (for left channel / mono)
```

---

## 💻 Software Requirements

- [Arduino IDE](https://www.arduino.cc/en/software) 2.x
- **esp32 by Espressif Systems** board package — version **3.3.7 or later**
  - Install via: `Tools → Board → Boards Manager → search "esp32"`
- Chrome or Edge browser (Web Serial API required)

---

## 🚀 Getting Started

### 1. Flash the ESP32

1. Open `sketch_jan21a.ino` in Arduino IDE
2. Select board: `Tools → Board → ESP32 Dev Module`
3. Select the correct COM port
4. Click **Upload**

### 2. Open the Dashboard

1. Open `index.html` in **Chrome** or **Edge**
2. Click **Connect ESP32**
3. Select your COM port from the browser dialog
4. Data streams in real time

---

## 📁 Project Structure

```
ESP32-Based-Air-and-Sound-Pollution-Detector/
├── sketch_jan21a/
│   └── sketch_jan21a.ino   # ESP32 firmware
└── index.html              # Web dashboard (no server needed)
```

---

## 📡 Serial Data Format

The ESP32 outputs two lines per second over Serial at **115200 baud**:

```
air:142.0
sound:63.4
```

The dashboard parses these lines using the Web Serial API directly in the browser.

---

## 🔬 How It Works

### Air Quality (MQ2)
- Uses the new `adc_oneshot` API (ESP-IDF v5 / Arduino core v3+) to avoid conflicts with the I2S legacy ADC driver
- Raw ADC value (0–4095) mapped to 0–3000 µg/m³ range
- Thresholds: Good ≤500 | Moderate ≤1500 | Unhealthy >1500

### Sound Level (I2S Mic)
- Reads 256 samples per measurement using the new `i2s_std` API
- Computes **RMS amplitude** across the sample buffer
- Converts to decibels: `dB = 20 × log10(rms) + 120`
- Constrained to 0–120 dB range
- Threshold: Safe ≤85 dB (NIOSH limit) | Danger >85 dB

---

## ⚠️ Known Issues & Fixes

### `ADC: CONFLICT! driver_ng is not allowed to be used with the legacy driver`
This happens when using `analogRead()` / `analogSetAttenuation()` alongside I2S on ESP32 Arduino core v3+. Fixed by:
- Using `esp_adc/adc_oneshot.h` instead of `analogRead()`
- Using `driver/i2s_std.h` instead of `driver/i2s.h`

### Web Serial not working
Web Serial API requires **Chrome or Edge**. Firefox is not supported.

---

## 📊 Dashboard Features

- Live line charts with threshold annotation lines
- Min / Avg / Max stats per sensor
- Quality bar with color-coded status
- Uptime counter and data point tracker
- Clear chart button per sensor
- Graceful connect / disconnect handling

---

## 📄 License

MIT License — free to use, modify, and distribute.

---

## 🙌 Credits

Built with:
- [Chart.js](https://www.chartjs.org/)
- [chartjs-plugin-annotation](https://www.chartjs.org/chartjs-plugin-annotation/)
- [ESP-IDF / Arduino ESP32 Core](https://github.com/espressif/arduino-esp32)
- [Web Serial API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API)
