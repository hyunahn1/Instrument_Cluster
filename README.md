# PiRacer Speed Sensor System

Real-time speed measurement system for PiRacer autonomous vehicle using Arduino and LM393 IR speed sensor.

## 📋 Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Pin Configuration](#pin-configuration)
- [Installation](#installation)
- [PlatformIO Usage Guide](#platformio-usage-guide)
- [Usage](#usage)
- [Output Example](#output-example)
- [Project Structure](#project-structure)
- [Build Process](#build-process)
- [Technical Details](#technical-details)
- [Troubleshooting](#troubleshooting)
- [Future Improvements](#future-improvements)
- [License](#license)

## 🎯 Overview

Speed measurement system for PiRacer autonomous vehicle using LM393 infrared speed sensor and Arduino. Implements interrupt-based pulse counting for real-time speed monitoring via serial communication.

## Features

- Interrupt-based pulse counting for accurate measurement
- Real-time speed monitoring (500ms update interval)
- Serial output for data logging
- Documented code structure

## 🔧 Hardware Requirements

| Component | Description |
|-----------|-------------|
| **Microcontroller** | Arduino Uno/Nano (ATmega328P) |
| **Speed Sensor** | LM393 IR Speed Sensor Module |
| **Platform** | PiRacer or similar RC car |
| **Cable** | USB A-B cable (for programming) |
| **Power** | 5V via USB or external power supply |

### LM393 Speed Sensor Specifications
- **Operating Voltage**: 3.3V - 5V
- **Output**: Digital (DO) and Analog (AO)
- **Detection**: Infrared reflection-based
- **Adjustable Sensitivity**: Built-in potentiometer

## 📌 Pin Configuration

### Sensor to Arduino Connections

```
┌─────────────┐         ┌──────────────┐
│  LM393      │         │  Arduino Uno │
│  Sensor     │         │              │
├─────────────┤         ├──────────────┤
│  VCC   ────────────── │  5V          │
│  GND   ────────────── │  GND         │
│  DO    ────────────── │  Pin 2 (INT0)│
└─────────────┘         └──────────────┘
```

| LM393 Pin | Arduino Pin | Description |
|-----------|-------------|-------------|
| VCC | 5V | Power supply |
| GND | GND | Ground |
| DO | Digital Pin 2 | Digital output (interrupt) |

> **Note**: Pin 2 is used because it supports hardware interrupts on Arduino Uno.

## 💾 Installation

### Prerequisites
- [PlatformIO](https://platformio.org/) installed (recommended)
- OR [Arduino IDE](https://www.arduino.cc/en/software) 1.8+

### Method 1: PlatformIO (Recommended)

```bash
# Clone the repository
git clone https://github.com/yourusername/piracer-speed-sensor.git

# Navigate to project directory
cd piracer-speed-sensor

# Build and upload
pio run -t upload

# Open serial monitor
pio device monitor
```

### Method 2: Arduino IDE

1. Download or clone this repository
2. Open `src/main.cpp` in Arduino IDE
3. Select **Tools** → **Board** → **Arduino Uno**
4. Select correct **Port** under **Tools** → **Port**
5. Click **Upload** button
6. Open **Serial Monitor** (Tools → Serial Monitor)
7. Set baud rate to **9600**

## 🛠️ PlatformIO Usage Guide

### What is PlatformIO?

PlatformIO is a professional development platform for embedded systems. It provides:
- **Unified toolchain** - automatic tool management
- **Library management** - easy dependency handling
- **Multiple board support** - 1000+ boards
- **Built-in debugging** - better than Arduino IDE
- **Command-line tools** - automation friendly

### Installing PlatformIO

#### Option 1: VSCode Extension (Recommended)
1. Install [Visual Studio Code](https://code.visualstudio.com/)
2. Open VSCode Extensions (Ctrl+Shift+X / Cmd+Shift+X)
3. Search for "PlatformIO IDE"
4. Click Install

#### Option 2: Command Line
```bash
# Install PlatformIO Core
pip install platformio

# Verify installation
pio --version
```

### Essential PlatformIO Commands

```bash
# ============ Project Management ============
# Initialize new project
pio project init --board uno

# Install libraries
pio lib install "Library Name"

# Update platforms and libraries
pio upgrade


# ============ Building ============
# Clean previous builds
pio run -t clean

# Compile project
pio run

# Compile with verbose output
pio run -v

# Check code size
pio run -t size


# ============ Uploading ============
# Compile and upload to Arduino
pio run -t upload

# Upload to specific port
pio run -t upload --upload-port /dev/cu.usbserial-1420

# List available serial ports
pio device list


# ============ Serial Monitor ============
# Open serial monitor
pio device monitor

# Monitor with specific baud rate
pio device monitor -b 9600

# Monitor on specific port
pio device monitor -p /dev/cu.usbserial-1420

# Exit serial monitor: Ctrl+C


# ============ Combined Commands ============
# Upload and immediately open monitor
pio run -t upload && pio device monitor

# Clean, build, upload, and monitor
pio run -t clean && pio run -t upload && pio device monitor
```

### Project Configuration (platformio.ini)

```ini
[platformio]
default_envs = uno              ; Default environment to build

[env:uno]
platform = atmelavr             ; AVR platform for Arduino
board = uno                     ; Target board
framework = arduino             ; Use Arduino framework

monitor_speed = 9600            ; Serial monitor baud rate
monitor_filters = default, time ; Add timestamps to serial

build_flags = 
    -D VERSION=1.0.0           ; Define VERSION macro
    -Wall                       ; Enable all warnings

upload_speed = 115200           ; Upload baud rate
```

### Common Workflows

**Daily Development Cycle:**
```bash
# 1. Make code changes in src/main.cpp
# 2. Build and test
pio run -t upload && pio device monitor
# 3. See output, press Ctrl+C to stop
# 4. Repeat
```

**First Time Setup:**
```bash
# 1. Clone repository
git clone <repo-url>
cd piracer-speed-sensor

# 2. PlatformIO auto-installs dependencies
pio run

# 3. Connect Arduino and upload
pio run -t upload
```

**Debugging Build Issues:**
```bash
# Clean everything and rebuild
pio run -t clean
pio run -v

# Check project configuration
pio project config

# Update all tools
pio upgrade
pio platform update
```

### PlatformIO vs Arduino IDE

| Feature | PlatformIO | Arduino IDE |
|---------|-----------|-------------|
| **Auto-complete** | ✅ Excellent | ❌ Limited |
| **Debugging** | ✅ Built-in | ❌ None |
| **Build speed** | ✅ Fast | ⚠️ Slower |
| **Library management** | ✅ Automated | ⚠️ Manual |
| **Multiple boards** | ✅ Easy | ⚠️ Complex |
| **Command line** | ✅ Full support | ❌ Limited |
| **Learning curve** | ⚠️ Steeper | ✅ Easier |

## 🚀 Usage

### Step 1: Hardware Setup
1. Connect LM393 sensor to Arduino following pin configuration
2. Mount sensor on PiRacer chassis near wheel
3. Adjust sensor position for optimal detection
4. Connect Arduino to computer via USB

### Step 2: Upload Code
```bash
pio run -t upload
```

### Step 3: Monitor Output
```bash
pio device monitor
```
Or use Arduino IDE Serial Monitor at 9600 baud.

### Step 4: Test
- Drive your PiRacer
- Observe speed readings in serial monitor
- Speed will be displayed as pulses per second

### Sensor Calibration
The LM393 module has a **blue potentiometer** for sensitivity adjustment:
- Turn **clockwise** to decrease sensitivity
- Turn **counter-clockwise** to increase sensitivity
- Adjust until reliable pulse detection is achieved

## 📊 Output Example

```
=========================================
   PiRacer Speed Sensor System v1.0     
=========================================
Sensor: LM393 IR Speed Sensor
Platform: Arduino Uno (ATmega328P)
Update Interval: 500 ms
=========================================
Starting measurements...

Pulses: 0 | Speed: 0.00 pulse/s | Time: 1.50 s
Pulses: 12 | Speed: 24.00 pulse/s | Time: 2.00 s
Pulses: 18 | Speed: 36.00 pulse/s | Time: 2.50 s
Pulses: 15 | Speed: 30.00 pulse/s | Time: 3.00 s
```

## 📁 Project Structure

```
piracer-speed-sensor/
├── src/
│   └── main.cpp              # Main source code (speed sensor logic)
├── include/                  # Header files (if any)
├── lib/                      # Custom libraries (empty for now)
├── test/                     # Unit tests (empty for now)
├── docs/                     # Additional documentation
│   ├── HARDWARE.md          # Hardware setup and wiring guide
│   ├── BUILD_PROCESS.md     # Detailed build pipeline explanation
│   └── API.md               # Code structure and function reference
├── platformio.ini            # PlatformIO configuration
├── README.md                 # This file (project overview)
├── CHANGELOG.md              # Version history and release notes
├── LICENSE                   # MIT License
└── .gitignore               # Git ignore rules
```

### Key Files Description

| File | Purpose |
|------|---------|
| `src/main.cpp` | Core speed sensor implementation |
| `platformio.ini` | Build configuration and dependencies |
| `README.md` | Main documentation (you're reading it!) |
| `CHANGELOG.md` | Development history and version tracking |
| `docs/HARDWARE.md` | Wiring diagrams and sensor setup |
| `docs/BUILD_PROCESS.md` | How compilation works |
| `docs/API.md` | Function reference and code documentation |
| `LICENSE` | Open source license (MIT) |

## 🏗️ Build Process

### How Code Becomes Arduino Program

Ever wondered what happens when you click "Upload"? Here's the complete journey from code to running program:

```
Your Code (main.cpp)
    ↓
Preprocessing (#include expansion)
    ↓
Compilation (C++ → Assembly)
    ↓
Assembly (Assembly → Machine Code)
    ↓
Linking (Combine all code)
    ↓
Format Conversion (ELF → HEX)
    ↓
Upload (HEX → Arduino Flash)
    ↓
✅ Program Running!
```

### Build Pipeline Overview

```
┌─────────────────┐
│  1. Source      │  src/main.cpp + Arduino libraries
└────────┬────────┘
         ↓
┌─────────────────┐
│  2. Compiler    │  avr-g++ processes code
│  Frontend       │  - Preprocessor (#include, #define)
└────────┬────────┘  - Parser (checks syntax)
         ↓           - IR generation
┌─────────────────┐
│  3. Compiler    │  avr-g++ backend
│  Backend        │  - Generates AVR assembly
└────────┬────────┘  - Optimizes for size (-Os)
         ↓
┌─────────────────┐
│  4. Assembler   │  avr-as creates object files (.o)
└────────┬────────┘  - Binary machine code
         ↓           - One .o per source file
┌─────────────────┐
│  5. Archiver    │  avr-ar bundles Arduino libraries
└────────┬────────┘  - Creates libFrameworkArduino.a
         ↓
┌─────────────────┐
│  6. Linker      │  avr-g++ combines everything
└────────┬────────┘  - Links your .o + libraries
         ↓           - Resolves addresses → firmware.elf
┌─────────────────┐
│  7. objcopy     │  Format conversion
└────────┬────────┘  - ELF → Intel HEX format
         ↓           - Creates firmware.hex
┌─────────────────┐
│  8. avrdude     │  Upload via USB
└─────────────────┘  - Writes to Arduino flash memory
```

### Key Stages Explained

**Stage 1: Preprocessing**
- Expands `#include <Arduino.h>` (inserts ~5000 lines)
- Replaces `#define` macros with values
- Processes conditional compilation

**Stage 2-3: Compilation**
- Checks C++ syntax and semantics
- Converts code to AVR assembly language
- Applies optimizations (`-Os` = optimize for size)

**Stage 4-5: Assembly & Archiving**
- Converts assembly to binary machine code
- Creates reusable library archives
- Each source file becomes `.o` object file

**Stage 6: Linking**
- Combines your code + Arduino libraries
- Resolves function calls to addresses
- Determines memory layout (Flash/RAM)
- Creates `firmware.elf` executable

**Stage 7: Format Conversion**
- Strips debug information
- Converts to Intel HEX format (uploadable format)
- `firmware.hex` ready for Arduino

**Stage 8: Upload**
- Arduino bootloader receives HEX file
- Writes to flash memory
- Verifies successful upload
- Resets and runs your program

### Memory Layout

When your program is uploaded:

```
Arduino Uno Memory Map

┌─────────────────────────┐  0x0000
│   Interrupt Vectors     │  (256 bytes)
├─────────────────────────┤
│   Your Program Code     │
│   (Flash: 32KB)         │  ← firmware.hex goes here
│   - setup()             │
│   - loop()              │
│   - ISR_countPulse()    │
│   - Arduino libraries   │
├─────────────────────────┤
│   Bootloader            │  (512 bytes, protected)
└─────────────────────────┘  0x7FFF

┌─────────────────────────┐  0x0100
│   Global Variables      │
│   (RAM: 2KB)            │  ← Data loaded at boot
│   - g_pulseCount        │
│   - g_currentSpeed      │
│   - Serial buffers      │
├─────────────────────────┤
│   Stack (grows down)    │  ← Function calls, locals
│          ↓              │
│                         │
│          ↑              │
│   Heap (grows up)       │  ← Dynamic allocation
└─────────────────────────┘  0x08FF
```

### Build Output Explained

When you run `pio run`, you see:

```
RAM:   [=         ]   9.8% (used 200 bytes from 2048 bytes)
Flash: [==        ]  10.2% (used 3296 bytes from 32256 bytes)
```

**What this means:**
- **RAM**: Global variables + buffers = 200 bytes
- **Flash**: Your program + libraries = 3296 bytes
- **Remaining**: 1848 bytes RAM, 28960 bytes Flash available

### Behind the Commands

**What `pio run` actually does:**
```bash
# 1. Preprocess
avr-g++ -E main.cpp -o main.cpp.i

# 2. Compile to assembly
avr-g++ -S main.cpp.i -o main.cpp.s

# 3. Assemble to object
avr-g++ -c main.cpp.s -o main.cpp.o

# 4. Link
avr-g++ main.cpp.o libArduino.a -o firmware.elf

# 5. Convert to HEX
avr-objcopy -O ihex firmware.elf firmware.hex
```

**What `pio run -t upload` adds:**
```bash
# Upload via avrdude
avrdude -p atmega328p -c arduino -P /dev/cu.usbserial \
  -U flash:w:firmware.hex:i
```

### Optimization Impact

Our code with different optimization flags:

| Flag | Flash Used | RAM Used | Description |
|------|-----------|----------|-------------|
| `-O0` | 4,820 bytes | 212 bytes | No optimization (debug) |
| `-Os` | 3,296 bytes | 200 bytes | Size optimized (default) ✅ |
| `-O2` | 4,100 bytes | 204 bytes | Speed optimized |
| `-O3` | 4,650 bytes | 208 bytes | Maximum speed |

**Why `-Os` is default**: Arduino has limited memory (32KB flash), so size optimization is critical.

### Viewing Intermediate Files

Want to see what's happening under the hood?

```bash
# View preprocessed code
pio run -v 2>&1 | grep "\.i"

# View assembly code
avr-objdump -d .pio/build/uno/firmware.elf > assembly.txt

# View symbol table (all functions and addresses)
avr-nm .pio/build/uno/firmware.elf

# View detailed memory usage
avr-size -C --mcu=atmega328p .pio/build/uno/firmware.elf
```

### 📚 Learn More

For complete details, see [docs/BUILD_PROCESS.md](docs/BUILD_PROCESS.md)

## 🔬 Technical Details

### Interrupt Service Routine (ISR)
The system uses Arduino's hardware interrupt capability on pin 2 (INT0). The ISR is triggered on every **FALLING edge** (HIGH to LOW transition) of the sensor signal.

**Advantages:**
- No polling required
- Fast response time
- No missed pulses
- Efficient CPU usage

### Speed Calculation
```
Speed (pulse/s) = Pulse Count / Time Interval (seconds)
```

With a 500ms update interval:
```
Speed = Pulse Count / 0.5 = Pulse Count × 2
```

### Memory Usage
```
RAM:   ~200 bytes (1% of 2KB)
Flash: ~3KB (9% of 32KB)
```

### Timing Accuracy
- **Update interval**: 500ms
- **Timing function**: `millis()` (1ms resolution)
- **Max counting rate**: ~4000 pulses/second (theoretical)

## 🐛 Troubleshooting

### No output on serial monitor
- Check USB connection
- Verify baud rate is set to 9600
- Ensure correct COM port is selected

### Speed always shows 0
- Check sensor wiring (VCC, GND, DO)
- Verify sensor is powered (LED should be on)
- Adjust sensor sensitivity with potentiometer
- Ensure sensor is positioned correctly near wheel
- Check if wheel pattern provides contrast for detection

### Erratic readings
- Sensor may be too sensitive - adjust potentiometer
- Check for loose connections
- Ensure stable power supply
- Move sensor away from electrical noise sources

### Compilation errors
- Update PlatformIO/Arduino IDE to latest version
- Verify board selection is correct
- Check for syntax errors in modified code

## 🔮 Future Improvements

- [ ] **Speed unit conversion** (pulses/s → km/h or m/s)
- [ ] **LCD display integration** for standalone operation
- [ ] **SD card data logging** for long-term analysis
- [ ] **WiFi/Bluetooth module** for wireless monitoring
- [ ] **ROS integration** for autonomous navigation
- [ ] **Dual sensor support** for differential speed measurement
- [ ] **OLED display** for real-time visualization
- [ ] **Calibration wizard** for automatic setup
- [ ] **Web dashboard** for remote monitoring

## 📄 License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

*Last Updated: February 12, 2026*
