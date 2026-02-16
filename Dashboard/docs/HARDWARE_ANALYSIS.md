# PiRacer Hardware Analysis and Dashboard Implementation Feasibility Review

## 📅 Date: 2026-02-16
## 👤 Author: Ahn Hyunjun
## 🎯 Purpose: Analysis of actual hardware constraints for Dashboard project

---

## 1. PiRacer Hardware Status

### 1.1 Current Sensor Configuration
- **LM393 IR Speed Sensor**: Speed measurement (Pulse counting)
- **Arduino Uno**: Sensor data collection and serial transmission
- **Raspberry Pi 4**: Data reception and dashboard display

### 1.2 PiRacer Vehicle Characteristics
- **Drive System**: DC electric motor (forward/reverse)
- **Gears**: None (single speed, direction only)
- **Steering**: Servo motor (steering angle)
- **Sensors**: Currently only speed sensor installed

---

## 2. Measurable Items Analysis

### 2.1 ✅ Immediately Available

| Item | Sensor/Method | Data | Difficulty |
|------|---------------|------|------------|
| **Current Speed** | LM393 IR Sensor | Pulse/s → km/h conversion | ⭐ Complete |
| **Drive Time** | Software timer | Elapsed time since start | ⭐ Easy |
| **Drive Distance** | Speed integration calculation | Speed × time accumulation | ⭐⭐ Medium |
| **Max Speed** | Software recording | Session maximum value storage | ⭐ Easy |
| **Average Speed** | Software calculation | Total distance / total time | ⭐⭐ Medium |

### 2.2 ⚠️ Conditionally Implementable

| Item | Implementation Method | Accuracy | Additional Requirements |
|------|----------------------|----------|------------------------|
| **RPM (Motor rotation)** | Speed sensor data conversion | Medium | Gear ratio/wheel diameter calibration |
| **Battery Level** | Voltage sensor addition | High | Voltage divider circuit needed |
| **Direction Display** | Steering servo angle reading | High | Servo feedback signal connection |

### 2.3 ❌ Not Implementable (No Sensor)

| Item | Reason | Alternative |
|------|--------|-------------|
| **Engine Temperature** | No temperature sensor | System temp (CPU) or exclude |
| **Fuel Level** | Electric vehicle (no fuel) | Replace with battery level |
| **Gear Position** | No gears (forward/reverse only) | Direction display (F/R) or exclude |

---

## 3. RPM Measurement Feasibility In-Depth Analysis

### 3.1 Meaning of RPM
- **Regular Cars**: Engine crankshaft rotation speed (1,000~8,000 RPM)
- **Electric Vehicles/PiRacer**: Motor rotation speed or wheel rotation speed

### 3.2 RPM Calculation Method for PiRacer

#### Method 1: Wheel RPM Calculation Using Speed Sensor ✅ **Recommended**

```
Wheel RPM = (Pulse/s × 60) / (Sensor pulses per rotation)
```

**Required Information:**
- Sensor pulse count per 1 wheel rotation (calibration needed)
- Current Pulse/s data (already measuring)

**Example:**
- Sensor detects 20 pulses per 1 wheel rotation
- Currently measuring 100 pulse/s
- Wheel RPM = (100 × 60) / 20 = **300 RPM**

#### Method 2: Add Separate Sensor to Motor ⚠️ **Hardware Addition Required**

- Mount Hall Effect sensor on motor shaft
- Use additional Arduino interrupt pin
- More accurate but requires hardware work

### 3.3 Conclusion: RPM Implementation Possible ✅

**Recommended Method:**
- Convert current speed sensor data to wheel RPM
- Can be implemented without additional sensors with calibration only
- Note: This is "wheel RPM" not "motor RPM"

---

## 4. Max Speed Record Reset Method

### 4.1 Reset Scenarios

| Scenario | Implementation Method | Pros & Cons |
|----------|----------------------|-------------|
| **Manual Button** | Add "Reset" button in GUI | 👍 Intuitive, 👎 Click required |
| **Auto Session Reset** | Auto reset after 5 min stop | 👍 Convenient, 👎 Unintended reset |
| **Program Restart** | Reset on app exit | 👍 Simple, 👎 Restart required |
| **Keyboard Shortcut** | Reset on 'R' key press | 👍 Fast, 👎 Typo possible |
| **File Save** | Save on exit, load on start | 👍 Record preserved, 👎 Complex |

### 4.2 Recommended Implementation: Multi-level Approach

**Phase 1 (Basic):**
```cpp
// Reset on program start
maxSpeed = 0.0;
```

**Phase 2 (GUI Button):**
```cpp
// On "Reset Max Speed" button click
void onResetButtonClicked() {
    maxSpeed = 0.0;
    updateDisplay();
}
```

**Phase 3 (Session Management):**
```cpp
// Optional: Auto session management
if (speed == 0 && idleTime > 300) {  // 5 min stop
    startNewSession();  // Start new session, reset record
}
```

### 4.3 Data Storage (Optional)

```cpp
// Save records in JSON format
{
  "session_date": "2026-02-16",
  "max_speed": 25.5,
  "total_distance": 1.8,
  "total_time": 120
}
```

---

## 5. Final Dashboard Implementation Items

### 5.1 Required Implementation (Phase 1)

| Item | Display Location | Data Source |
|------|------------------|-------------|
| **Current Speed** | Central gauge | LM393 sensor |
| **Drive Time** | Bottom right | Software timer |
| **Max Speed** | Bottom left | Memory storage |

### 5.2 Optional Implementation (Phase 2)

| Item | Display Location | Notes |
|------|------------------|-------|
| **Wheel RPM** | Left gauge | Speed data conversion |
| **Drive Distance** | Bottom right | Speed integration calculation |
| **Average Speed** | Bottom center | Statistics calculation |
| **Direction Display** | Top center | F(Forward) / R(Reverse) |

### 5.3 Excluded Items

| Item | Exclusion Reason | Alternative |
|------|------------------|-------------|
| ~~Temperature gauge~~ | No sensor | CPU temp or exclude |
| ~~Fuel gauge~~ | Electric vehicle | Battery display (future) |
| ~~Gear display~~ | No gears | Direction (F/R) display |

---

## 6. Calibration Requirements

### 6.1 Speed Conversion Coefficient

```cpp
// Actual measurement required: Pulse/s → km/h
float SPEED_CALIBRATION = 0.72;  // Example value
float speedKmh = pulsePerSec * SPEED_CALIBRATION;
```

**Measurement Method:**
1. Measure actual straight distance (e.g., 10m)
2. Record driving time and pulse count
3. Calculate: (distance/time) / Pulse/s = conversion coefficient

### 6.2 RPM Conversion Coefficient

```cpp
// Actual measurement required: Pulses per 1 wheel rotation
int PULSES_PER_REVOLUTION = 20;  // Example value
float wheelRPM = (pulsePerSec * 60.0) / PULSES_PER_REVOLUTION;
```

**Measurement Method:**
1. Manually rotate wheel exactly 1 revolution
2. Count pulses
3. Use average value after multiple repetitions

---

## 7. Additional Discussion Items

### 7.1 Forward/Reverse Detection

**Problem:**
- Current sensor measures speed magnitude only (no direction)
- Cannot distinguish forward 20km/h vs reverse 20km/h

**Solution (Confirmed):**
✅ **Use PiRacer API** (No additional sensor needed!)

```python
from piracer.vehicles import PiRacerStandard
piracer = PiRacerStandard()

# In set_throttle_percent() method:
# value > 0: Forward
# value < 0: Reverse
# value = 0: Neutral
```

**Implementation Method:**
- Track last throttle value in Python bridge
- Return direction string based on value ("F", "R", "N")
- Qt receives and displays on screen

**Note:** Cannot read actual throttle value, but possible by tracking the value set by main program controlling PiRacer

### 7.2 Battery Level Display

**Implementation Method (Confirmed):**
✅ **Direct Use of PiRacer API** (No additional circuit needed!)

```python
from piracer.vehicles import PiRacerStandard
piracer = PiRacerStandard()

# Battery monitoring via INA219 sensor
voltage = piracer.get_battery_voltage()      # Voltage (V)
current = piracer.get_battery_current()      # Current (mA)
power = piracer.get_power_consumption()      # Power (W)

# Percent calculation (LiPo 2S based)
V_MIN = 6.4   # 0%
V_MAX = 8.4   # 100%
percent = ((voltage - V_MIN) / (V_MAX - V_MIN)) * 100
```

**Sensor:**
- **INA219**: Voltage/current measurement IC (PiRacer built-in)
- I2C address: 0x41 (Standard) / 0x42 (Pro)
- Accuracy: High

**Additional Features:**
- Current consumption monitoring (mA)
- Power consumption calculation (W)
- Real-time battery status tracking

### 7.3 Screen Resolution and Hardware

**Confirmation Needed:**
- Display connected to Raspberry Pi?
  - Official 7-inch touchscreen? (800×480)
  - HDMI monitor? (1920×1080)
  - Other?

---

## 8. Next Steps

### 8.1 Immediate Decision Required
- [ ] Include RPM display
- [ ] Choose max speed reset method
- [ ] Include battery level display
- [ ] Confirm screen resolution

### 8.2 Calibration Work
- [ ] Actual measurement of speed conversion coefficient
- [ ] Actual measurement of RPM conversion coefficient
- [ ] Wheel circumference measurement

### 8.3 Documents to be Written
- [ ] Implementation Plan (IMPLEMENTATION_PLAN.md)
- [ ] Feature Specification (SPECIFICATION.md)
- [ ] Verification Plan (VERIFICATION_PLAN.md)

---

## Appendix: Reference Materials

### A. PiRacer Specifications (Confirmed)
- Wheel diameter: ~65mm (measurement needed)
- Max speed: ~30 km/h (estimate)
- Battery: 7.4V LiPo 2S
- Motor: DC brushed motor
- Battery monitor: INA219 (I2C)
- Display: SSD1306 OLED 128x32 (I2C)

### B. Sensor Specifications

#### Speed Sensor
- LM393 IR Speed Sensor
- Detection distance: 2-30mm
- Output: Digital (HIGH/LOW)
- Trigger: FALLING edge
- Connection: Arduino Digital Pin 2

#### Battery Monitor
- INA219 Power Monitor
- Measurement range: 0-26V, ±3.2A
- I2C address: 0x41 (Standard) / 0x42 (Pro)
- Accuracy: ±0.5% (voltage), ±1% (current)
- Connection: Raspberry Pi I2C

### C. PiRacer API Reference (piracer-py)

#### Official GitHub
- Repository: https://github.com/SEA-ME/piracer_py
- Version: 0.2.0+

#### Main Methods

```python
from piracer.vehicles import PiRacerStandard  # or PiRacerPro

piracer = PiRacerStandard()

# Battery information
voltage = piracer.get_battery_voltage()      # float (V)
current = piracer.get_battery_current()      # float (mA)  
power = piracer.get_power_consumption()      # float (W)

# Display
display = piracer.get_display()              # SSD1306_I2C (128x32)

# Vehicle control
piracer.set_throttle_percent(value)   # -1.0 ~ 1.0 (reverse ~ forward)
piracer.set_steering_percent(value)   # -1.0 ~ 1.0 (left ~ right)
```

#### Dependencies
- adafruit-circuitpython-pca9685 (PWM controller)
- adafruit-circuitpython-ina219 (power monitor)
- adafruit-circuitpython-ssd1306 (OLED display)
- opencv-python (camera, optional)
