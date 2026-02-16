# PiRacer Luxury Sports Dashboard 🏎️

Luxury sports car style dashboard system for PiRacer

![Version](https://img.shields.io/badge/version-1.0.0-blue)
![Qt](https://img.shields.io/badge/Qt-5.15%2B-green)
![C++](https://img.shields.io/badge/C%2B%2B-17-orange)
![Python](https://img.shields.io/badge/Python-3.9%2B-yellow)
![License](https://img.shields.io/badge/license-MIT-lightgrey)

## 📋 Project Overview

Real-time dashboard system for PiRacer autonomous vehicle. Luxury sports car style UI inspired by Porsche 911 / BMW M Series, intuitively displaying speed, RPM, battery level, and more.

### Key Features

- 🎯 **Central Speedometer**: Hybrid analog needle + digital numbers
- 📊 **Wheel RPM**: Real-time rotation speed display
- 🔋 **Battery Monitoring**: Voltage and percentage display (color-coded)
- ⏱️ **Drive Time**: Real-time timer
- 🏆 **Max Speed Record**: Session maximum value save and reset
- 🔄 **Direction Display**: Forward/Reverse indicator (future implementation)
- 🎨 **Premium UI**: 60 FPS smooth animations

## 🖥️ System Requirements

### Hardware
- **Raspberry Pi 4** (4GB RAM recommended)
- **7-inch DSI Touchscreen** (1200×400)
- **Arduino Uno** + LM393 IR Speed Sensor
- **PiRacer Platform**

### Software
- Raspberry Pi OS (Bullseye or later)
- Qt 5.15+ or Qt 6
- Python 3.9+
- piracer-py library

## 📐 Screen Layout

```
┌────────────────────────────────────────────────────────────┐
│  [RPM]      Central Speedometer (Needle + Number)  [Dir]  │
│  3200         ╭─────────────╮                       F     │
│  Wheel        │    145      │                   Forward   │
│  RPM          │   km/h      │                            │
│               ╰─────────────╯              [Battery]      │
│  [Battery]                                   85%         │
│  ████ 85%                                   7.8V         │
│                                                          │
│                                            [Time]        │
│                                           00:12:34       │
├───────────────────────────────────────────────────────────┤
│  Max Speed: 28.5 km/h                    [RESET]         │
└────────────────────────────────────────────────────────────┘
```

## 🚀 Quick Start

### 1. Clone Repository and Navigate
```bash
cd /Users/ahnhyunjun/Documents/PlatformIO/Projects/ADUINO/Dashboard
```

### 2. Install Dependencies

#### Install Qt
```bash
sudo apt update
sudo apt install -y qt5-default qtcreator
sudo apt install -y libqt5serialport5-dev
sudo apt install -y cmake build-essential git
```

#### Python Libraries
```bash
sudo pip3 install piracer-py pyserial
```

### 3. Build (CMake, recommended)

```bash
mkdir build && cd build
cmake ..
cmake --build . -j4
```

### 4. Run

```bash
# Serial port permission setup (one-time only)
sudo usermod -a -G dialout $USER
# Re-login required

# Execute (macOS bundle)
./build/PiRacerDashboard.app/Contents/MacOS/PiRacerDashboard
```

### Qt Creator (CMake)

1. Open `Dashboard/CMakeLists.txt`
2. Select a Qt 6 (or Qt 5) CMake Kit
3. Configure Project
4. Build and Run

## 📂 Project Structure

```
Dashboard/
├── docs/                      # Documentation
│   ├── HARDWARE_ANALYSIS.md  # Hardware analysis
│   ├── SPECIFICATION.md      # Feature specification
│   ├── IMPLEMENTATION_PLAN.md # Implementation plan
│   └── VERIFICATION_PLAN.md  # Verification plan
│
├── src/                      # C++ source code
│   ├── main.cpp             # Entry point
│   ├── MainWindow.h/cpp     # Main window
│   ├── widgets/             # Custom widgets
│   ├── serial/              # Serial communication
│   └── utils/               # Utilities
│
├── python/                  # Python bridge
│   └── piracer_bridge.py   # PiRacer data reading
│
├── resources/               # Resource files
│   ├── fonts/              # Fonts
│   └── icons/              # Icons
│
├── config/                 # Configuration files
│   └── calibration.json   # Calibration values
│
└── tests/                  # Test code
```

## 📊 Data Flow

```
Arduino (LM393) ──Serial──> Qt Dashboard ──┬──> Speedometer
                                           ├──> RPM Gauge
                                           └──> Max Speed

PiRacer Library ──Python──> Qt Dashboard ──┬──> Battery
                                           └──> Direction
```

## ⚙️ Configuration and Calibration

### calibration.json Example

```json
{
  "speed": {
    "pulses_per_second_to_kmh": 0.72
  },
  "rpm": {
    "pulses_per_revolution": 20
  },
  "battery": {
    "v_min": 6.4,
    "v_max": 8.4
  }
}
```

### Calibration Method

1. **Speed Coefficient Measurement**
   - Set accurate 10m straight distance
   - Record driving time and pulse count
   - `FACTOR = (distance/time) / pulsePerSec`

2. **RPM Coefficient Measurement**
   - Manually rotate wheel exactly 1 revolution
   - Count pulses
   - Use average value after 3 repetitions

See `docs/IMPLEMENTATION_PLAN.md` for details

## 🎨 Design Style

- **Color Palette**:
  - Background: `#0A0E1A` (Deep navy)
  - Main text: `#E8F0FF` (Bright white)
  - Accent: `#00D4FF` (Cyan blue)
  - Warning: `#FF3B3B` (Red)

- **Typography**:
  - Roboto Bold (Numbers)
  - Roboto Regular (Labels)
  - Roboto Mono (Time)

- **Animation**:
  - 60 FPS smooth transitions
  - 200ms Ease-out needle rotation
  - Blink effect on new record

## 🧪 Testing

### Unit Test Execution
```bash
cd build
ctest --verbose
```

### Manual Testing
- Serial communication: `minicom -D /dev/ttyUSB0 -b 9600`
- Python bridge: `python3 python/piracer_bridge.py`

See `docs/VERIFICATION_PLAN.md` for detailed test plan

## 📈 Performance Goals

- **FPS**: ≥ 55 (average)
- **CPU Usage**: < 20%
- **Memory**: < 100MB
- **Response Delay**: < 50ms (buttons)
- **Accuracy**: ±5% (speed), ±10% (RPM)

## 🐛 Known Limitations

- Direction detection: Currently fixed value (future: PWM signal reading)
- Temperature sensor: None (can substitute with CPU temp)
- Gear display: PiRacer has single speed (forward/reverse only)

## 🗺️ Roadmap

### Phase 1 (Current) - Basic Features
- [x] Hardware analysis
- [x] Feature specification
- [x] Implementation plan
- [x] Verification plan
- [ ] Qt project creation
- [ ] Basic UI implementation
- [ ] Serial communication
- [ ] Battery monitoring

### Phase 2 (Future)
- [ ] Data logging (CSV)
- [ ] Statistics screen (average speed, total distance)
- [ ] Settings screen
- [ ] Automatic direction detection
- [ ] Graph (speed change trend)

## 📚 Documentation

- [Hardware Analysis](docs/HARDWARE_ANALYSIS.md): PiRacer hardware constraints and feasibility review
- [Feature Specification](docs/SPECIFICATION.md): Detailed feature and UI specification
- [Implementation Plan](docs/IMPLEMENTATION_PLAN.md): Step-by-step implementation plan and schedule
- [Verification Plan](docs/VERIFICATION_PLAN.md): Test scenarios and verification criteria

## 🤝 Contributing

Issues and pull requests are welcome!

## 📄 License

MIT License - See [LICENSE](LICENSE) file for details

## 👤 Developer

**Ahn Hyunjun**
- GitHub: [@your-github](https://github.com/your-github)
- Email: your-email@example.com

## 🙏 Acknowledgments

- Qt Framework
- PiRacer Community
- Arduino Community

---

**⚠️ Warning**: This project is created for educational and development purposes. Do not use for actual road driving.

**📅 Last Updated**: 2026-02-16
