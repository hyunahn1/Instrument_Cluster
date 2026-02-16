# PiRacer Dashboard Feature Specification

## 📋 Document Information
- **Project Name**: PiRacer Luxury Sports Dashboard
- **Version**: 1.0.0
- **Date**: 2026-02-16
- **Author**: Ahn Hyunjun
- **Design Style**: Luxury Sports Car (Porsche 911 / BMW M Series Style)

---

## 1. System Overview

### 1.1 Purpose
Implementation of a luxury dashboard system that visually displays real-time driving data of PiRacer autonomous vehicle

### 1.2 System Configuration
```
┌─────────────┐    Serial     ┌──────────────┐    Python    ┌──────────────┐
│  Arduino    │─────────────→ │ Raspberry    │────────────→ │   Qt5/6      │
│  + LM393    │   (9600bps)   │  Pi 4        │   (Bridge)   │  Dashboard   │
│  Sensor     │               │  + PiRacer   │              │  GUI         │
└─────────────┘               └──────────────┘              └──────────────┘
     ↓                              ↓                              ↓
Speed Measurement          Battery/Direction            Visualization
```

### 1.3 Hardware Specifications
- **Display**: 7-inch DSI Touchscreen (1200×400 pixels)
- **Sensor**: LM393 IR Speed Sensor (Arduino connection)
- **Battery Monitor**: PiRacer library (Raspberry Pi)
- **Communication**: USB Serial (9600 baud, 8MHz)

---

## 2. Screen Layout Design

### 2.1 Resolution and Aspect Ratio
- **Resolution**: 1200 × 400 pixels (3:1 wide)
- **Orientation**: Landscape
- **DPI**: ~200 (based on 7-inch)

### 2.2 Layout Structure (unit: pixels)

```
┌────────────────────────────────────────────────────────────────────────────┐
│                           1200 pixels × 400 pixels                         │
├───────────────┬───────────────────────────────────────┬────────────────────┤
│               │                                       │                    │
│   LEFT PANEL  │         CENTER GAUGE                  │    RIGHT PANEL     │
│   (300×400)   │         (600×400)                     │    (300×400)       │
│               │                                       │                    │
│  ┌─────────┐  │  ╭─────────────────────────────╮     │   ┌─────────┐      │
│  │   RPM   │  │  │         ┌───────┐           │     │   │ DIRECTION│     │
│  │  3200   │  │  │      ╱   │       │   ╲       │     │   │    F     │     │
│  │ Wheel   │  │  │    ╱     │  145  │     ╲     │     │   │ Forward  │     │
│  │         │  │  │   │      │ km/h  │      │    │     │   └─────────┘      │
│  └─────────┘  │  │    ╲     │       │     ╱     │     │                    │
│               │  │      ╲   └───────┘   ╱       │     │   ┌─────────┐      │
│  ┌─────────┐  │  │         ─────────────         │     │   │ BATTERY │     │
│  │ BATTERY │  │  │       Speed Gauge             │     │   │  85%    │     │
│  │ ██████  │  │  │                               │     │   │ 7.8V    │     │
│  │  85%    │  │  ╰─────────────────────────────╯     │   └─────────┘      │
│  └─────────┘  │                                       │                    │
│               │                                       │   ┌─────────┐      │
└───────────────┴───────────────────────────────────────┴───│  TIME   │──────┤
│ Max Speed: 28.5 km/h              [RESET]              │ 00:12:34 │      │
└─────────────────────────────────────────────────────────┴─────────┘──────┘
```

### 2.3 Component Placement

| Area | Position (x,y,w,h) | Elements |
|------|-------------------|----------|
| **Left Panel** | (0, 0, 300, 400) | RPM gauge + Battery (auxiliary) |
| **Central Gauge** | (300, 0, 600, 400) | Main speedometer |
| **Right Panel** | (900, 0, 300, 400) | Direction + Battery(main) + Time |
| **Bottom Info Bar** | (0, 350, 1200, 50) | Max speed + Reset button |

---

## 3. Display Items Detailed Specification

### 3.1 Central Speedometer (Primary Display)

#### 3.1.1 Analog Gauge
- **Type**: Circular (270-degree arc)
- **Range**: 0 ~ 30 km/h
- **Scale**:
  - Major ticks: 0, 5, 10, 15, 20, 25, 30 (7 steps)
  - Minor ticks: 1 km/h unit (small ticks)
- **Red Zone**: 25~30 km/h (red display)
- **Needle**:
  - Color: White (#FFFFFF)
  - Length: 85% of radius
  - Thickness: 3px
  - Animation: Smooth rotation (Ease-out, 200ms)

#### 3.1.2 Digital Numbers
- **Position**: Center of gauge
- **Font**: 
  - Number: Roboto Bold, 72pt
  - Unit: Roboto Regular, 24pt
- **Color**: White (#E8F0FF)
- **Format**: `145` (integer) + `km/h` (unit)
- **Update Cycle**: 100ms (10 times per second)

### 3.2 Left RPM Gauge

#### 3.2.1 Wheel RPM Display
- **Type**: Semi-circle gauge (180 degrees)
- **Range**: 0 ~ 500 RPM
- **Calculation Formula**:
  ```cpp
  wheelRPM = (pulsePerSec * 60.0) / PULSES_PER_REVOLUTION
  ```
- **Display Format**: 
  - Number: `3200` (large font, 48pt)
  - Label: `Wheel RPM` (small font, 16pt)
- **Color**: Cyan Blue (#00D4FF)

#### 3.2.2 Battery Auxiliary Display (Small)
- **Position**: Below RPM
- **Size**: 100×60px
- **Content**: Simple battery icon + percentage

### 3.3 Right Information Panel

#### 3.3.1 Direction Display
- **Position**: Top
- **Size**: 150×80px
- **Display**:
  - `F` - Forward
  - `R` - Reverse
- **Color**:
  - Forward: Green (#00FF88)
  - Reverse: Red (#FF3B3B)
- **Font**: Roboto Bold, 48pt
- **Animation**: Fade effect on direction change

#### 3.3.2 Battery (Main)
- **Position**: Center
- **Size**: 200×120px
- **Composition**:
  - Battery icon (graphic)
  - Percentage number: `85%` (32pt)
  - Voltage: `7.8V` (20pt, gray)
- **Color Code**:
  - 80-100%: Green (#00FF88)
  - 50-79%: Yellow (#FFD700)
  - 20-49%: Orange (#FF8800)
  - 0-19%: Red + Blink (#FF3B3B)

#### 3.3.3 Drive Time
- **Position**: Bottom
- **Format**: `HH:MM:SS`
- **Example**: `00:12:34`
- **Font**: Roboto Mono, 28pt
- **Color**: White

### 3.4 Bottom Information Bar

#### 3.4.1 Max Speed Record
- **Position**: Left (0, 350, 500, 50)
- **Format**: `Max Speed: 28.5 km/h`
- **Font**: Roboto Regular, 20pt
- **Color**: Gold (#FFD700)
- **Animation**: Blink effect on new record (500ms)

#### 3.4.2 Reset Button
- **Position**: Right (1050, 355, 130, 40)
- **Text**: `RESET`
- **Style**: 
  - Background: Translucent gray (#333333AA)
  - Border: 1px white
  - Hover: Bright gray (#555555)
  - Click: Blue flash
- **Function**: Reset max speed record

---

## 4. Data Source Specification

### 4.1 Arduino → Raspberry Pi (Serial Communication)

#### 4.1.1 Data Format
```
Pulses: 42 | Speed: 84.00 pulse/s | Time: 12.34 s
```

#### 4.1.2 Parsing Rules
- Delimiter: `|`
- Extract Items:
  - `pulseCount`: Integer
  - `pulsePerSec`: Float (2 decimal places)
  - `timestamp`: Float (seconds)

#### 4.1.3 Communication Settings
```cpp
Port: /dev/ttyUSB0 (or /dev/ttyACM0)
Baud Rate: 9600
Data Bits: 8
Stop Bits: 1
Parity: None
Flow Control: None
```

### 4.2 PiRacer Library (Python)

#### 4.2.1 Battery Voltage
```python
from piracer.vehicles import PiRacerStandard
piracer = PiRacerStandard()
voltage = piracer.get_battery_voltage()  # 7.4V ~ 8.4V
```

#### 4.2.2 Battery Percentage Calculation
```python
# Based on LiPo 2S
V_MIN = 6.4   # 0% (discharge limit)
V_MAX = 8.4   # 100% (full charge)
percent = ((voltage - V_MIN) / (V_MAX - V_MIN)) * 100
percent = max(0, min(100, percent))  # Limit 0-100
```

#### 4.2.3 Direction Detection (Estimate)
```python
# Method 1: Read motor control signal
throttle = piracer.get_throttle_percent()
if throttle > 0:
    direction = "F"  # Forward
elif throttle < 0:
    direction = "R"  # Reverse
else:
    direction = "N"  # Neutral
```

---

## 5. Data Conversion and Calculation

### 5.1 Speed Conversion (Pulse/s → km/h)

#### 5.1.1 Conversion Formula
```cpp
float speedKmh = pulsePerSec * SPEED_CALIBRATION_FACTOR;
```

#### 5.1.2 Calibration Coefficient Measurement
**Actual Measurement Required:**
1. Set accurate straight distance (e.g., 10m)
2. Record drive time and pulse count
3. Calculate: `FACTOR = (distance / time) / pulsePerSec`

**Expected Range**: 0.5 ~ 1.0 (confirm after measurement)

### 5.2 RPM Calculation

#### 5.2.1 Wheel RPM Formula
```cpp
float wheelRPM = (pulsePerSec * 60.0) / PULSES_PER_REVOLUTION;
```

#### 5.2.2 PULSES_PER_REVOLUTION Measurement
1. Manually rotate wheel exactly 1 revolution
2. Check pulse count from serial output
3. Use average value after 3 repeated measurements

**Expected Value**: 10 ~ 30 pulses (depends on sensor mounting position)

### 5.3 Drive Distance Calculation

#### 5.3.1 Integration Calculation
```cpp
// Every update
float deltaTime = (currentTime - lastTime) / 1000.0;  // seconds
float deltaDistance = speedKmh * (deltaTime / 3600.0);  // km
totalDistance += deltaDistance;
```

#### 5.3.2 Accuracy Improvement
- Update cycle: 100ms (10 times per second)
- Accumulated error correction: Compare actual measurement every 1km

---

## 6. UI/UX Detailed Specification

### 6.1 Color Palette (Final)

| Element | Color Code | RGB | Usage |
|---------|-----------|-----|-------|
| **Background** | `#0A0E1A` | (10, 14, 26) | Entire background |
| **Main Text** | `#E8F0FF` | (232, 240, 255) | Numbers, labels |
| **Accent Blue** | `#00D4FF` | (0, 212, 255) | RPM, highlights |
| **Gold** | `#FFD700` | (255, 215, 0) | Max speed |
| **Green** | `#00FF88` | (0, 255, 136) | Forward, battery good |
| **Red** | `#FF3B3B` | (255, 59, 59) | Reverse, warning, red zone |
| **Gray** | `#7A8A9E` | (122, 138, 158) | Auxiliary text |
| **Silver** | `#C0C8D0` | (192, 200, 208) | Gauge border |

### 6.2 Typography

| Element | Font | Size | Weight | Color |
|---------|------|------|--------|-------|
| **Central Speed (Number)** | Roboto | 72pt | Bold | #E8F0FF |
| **Central Speed (Unit)** | Roboto | 24pt | Regular | #E8F0FF |
| **RPM Number** | Roboto | 48pt | Bold | #00D4FF |
| **RPM Label** | Roboto | 16pt | Regular | #7A8A9E |
| **Direction Display** | Roboto | 48pt | Bold | #00FF88 / #FF3B3B |
| **Battery (%)** | Roboto | 32pt | Bold | Contextual |
| **Time** | Roboto Mono | 28pt | Regular | #E8F0FF |
| **Max Speed** | Roboto | 20pt | Regular | #FFD700 |
| **Button** | Roboto | 18pt | Medium | #E8F0FF |

### 6.3 Animation Specification

#### 6.3.1 Needle Rotation (Speedometer)
```cpp
Duration: 200ms
Easing: QEasingCurve::OutCubic
Property: rotation angle (0° ~ 270°)
FPS: 60
```

#### 6.3.2 New Record Effect
```cpp
Type: Blink
Duration: 500ms (on) + 500ms (off) × 3 times
Color: #FFD700 ↔ #FFF700 (brighter)
```

#### 6.3.3 Direction Change Effect
```cpp
Type: Fade transition
Duration: 300ms
Opacity: 0 → 1
```

#### 6.3.4 Battery Warning (Below 20%)
```cpp
Type: Pulse
Duration: 1000ms (on) + 1000ms (off)
Repeat: Infinite
```

### 6.4 Touch Interaction

#### 6.4.1 Reset Button
```
Idle: Translucent gray
Hover: Bright gray (unnecessary if touch supported)
Pressed: Blue flash + Haptic feedback(optional)
Released: Reset max speed to 0 + confirmation effect
```

#### 6.4.2 Full Screen Gestures (Optional)
- Double tap: Toggle fullscreen
- Swipe down: Exit confirmation dialog

---

## 7. Performance Requirements

### 7.1 Response Speed
- **Data Reception**: Delay under 100ms
- **Screen Update**: 100ms cycle (10 FPS)
- **Animation**: Maintain 60 FPS
- **Button Response**: Within 50ms

### 7.2 Resource Usage
- **CPU Usage**: Average < 20% (Raspberry Pi 4 based)
- **Memory**: < 100MB
- **GPU Acceleration**: Qt OpenGL recommended

### 7.3 Stability
- **Continuous Operation Time**: Minimum 2 hours non-stop
- **Serial Disconnection**: Auto-reconnection attempt (5-second interval)
- **Data Loss**: Ignore up to 3 packets (then display "disconnected")

---

## 8. Extension Features (Phase 2, Optional)

### 8.1 Data Logging
```
File Format: CSV
Save Location: /home/pi/piracer_logs/
Filename: piracer_YYYYMMDD_HHMMSS.csv
Content:
  timestamp, speed_kmh, rpm, battery_percent, battery_voltage, direction
  12.34, 15.6, 312, 85, 7.8, F
  12.44, 16.2, 324, 85, 7.8, F
```

### 8.2 Statistics Information
- Average speed
- Total drive distance
- Battery consumption rate
- Session summary

### 8.3 Settings Screen
- Unit conversion (km/h ↔ mph)
- Color theme change
- Calibration value adjustment
- Data logging on/off

---

## 9. Constraints and Limitations

### 9.1 Hardware Constraints
- ❌ Engine temperature: No sensor
- ❌ Actual gears: Only forward/reverse exist
- ⚠️ Direction detection: Estimate-based (accuracy limited)
- ⚠️ RPM: Wheel RPM (not motor RPM)

### 9.2 Accuracy Limitations
- Speed: ±5% (calibration dependent)
- Distance: ±10% (accumulated error)
- Battery: ±5% (voltage fluctuation)

### 9.3 Usage Environment
- Daytime outdoor: Screen brightness may be insufficient
- Vibration: Touch accuracy may deteriorate
- Temperature: -10°C ~ 40°C recommended

---

## 10. Test Scenarios

### 10.1 Functionality Tests
- [ ] Speed display accuracy (actual measurement comparison)
- [ ] RPM calculation accuracy
- [ ] Battery display accuracy (multimeter comparison)
- [ ] Direction change detection
- [ ] Max speed record and reset
- [ ] Time display accuracy

### 10.2 UI Tests
- [ ] All element readability (7-inch screen)
- [ ] Animation smoothness
- [ ] Button touch response
- [ ] Color contrast (outdoor/indoor)

### 10.3 Stability Tests
- [ ] 2-hour continuous operation
- [ ] Serial cable unplug/plug
- [ ] Rapid acceleration/deceleration repetition
- [ ] Battery discharge scenario

---

## 11. Reference Documents
- `HARDWARE_ANALYSIS.md`: Hardware constraint analysis
- `IMPLEMENTATION_PLAN.md`: Implementation plan and schedule
- `VERIFICATION_PLAN.md`: Verification and test plan

---

**Approval**: ________________  
**Date**: 2026-02-16
