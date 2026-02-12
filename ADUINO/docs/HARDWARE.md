# Hardware Setup Guide

## 📋 Component List

### Required Components
| Item | Quantity | Specifications | Purpose |
|------|----------|----------------|---------|
| Arduino Uno/Nano | 1 | ATmega328P based | Main controller |
| LM393 Speed Sensor | 1 | 3.3V-5V, IR-based | Speed detection |
| USB Cable | 1 | Type A to B | Programming & power |
| Jumper Wires | 3 | Male-Female | Connections |

### Optional Components
| Item | Purpose |
|------|---------|
| External 5V Power Supply | Standalone operation |
| Breadboard | Prototyping |
| Mounting Brackets | Secure sensor placement |

## 🔌 Wiring Diagram

### Pin Connections

```
LM393 Sensor Module          Arduino Uno
┌─────────────────┐          ┌──────────────┐
│                 │          │              │
│  [LED]  [POT]   │          │              │
│                 │          │              │
│  VCC ●──────────┼──────────●─ 5V          │
│  GND ●──────────┼──────────●─ GND         │
│  DO  ●──────────┼──────────●─ Pin 2       │
│  AO  ● (unused) │          │              │
│                 │          │              │
└─────────────────┘          └──────────────┘
```

### Color Coding (if using colored wires)
- **Red** → VCC to 5V
- **Black** → GND to GND
- **Yellow/Green** → DO to Pin 2

## 🔧 Sensor Module Details

### LM393 Module Features
1. **Power LED**: Indicates module is powered
2. **Signal LED**: Blinks when detection occurs
3. **Potentiometer (Blue)**: Adjusts detection sensitivity
4. **IR LED**: Emits infrared light
5. **Photodiode**: Receives reflected IR light

### Module Pinout
```
┌───────────────┐
│   LM393       │
│   ┌─────┐     │
│   │ POT │     │  ← Sensitivity adjustment
│   └─────┘     │
│               │
│  VCC  ●       │
│  GND  ●       │
│  DO   ●       │  ← Digital Output
│  AO   ●       │  ← Analog Output (not used)
│               │
│  [IR LED]  [PD]│
└───────────────┘
```

## 📍 Physical Mounting

### Sensor Placement Guidelines

1. **Position**: Mount sensor 2-5mm from wheel surface
2. **Angle**: Perpendicular to wheel for best detection
3. **Stability**: Secure mounting to prevent vibration
4. **Clearance**: Ensure sensor doesn't interfere with wheel rotation

### Mounting Diagram
```
Side View:
                Sensor
                  ↓
        ┌─────────────┐
        │   LM393     │
        │  [IR] [PD]  │
        └─────┬───────┘
              │ 2-5mm gap
        ══════●══════    ← Wheel surface
        ║            ║
        ║   Wheel    ║
        ║            ║
        ══════════════
```

### Wheel Surface Requirements
- **Contrast**: Different colors or reflectivity patterns work best
- **Pattern**: Alternating dark/light sections ideal
- **Surface**: Clean, non-glossy surface for consistent detection

## ⚙️ Sensor Calibration

### Sensitivity Adjustment Steps

1. **Connect everything** and power on
2. **Open serial monitor** (9600 baud)
3. **Rotate wheel slowly** by hand
4. **Observe signal LED** on sensor module
5. **Adjust potentiometer**:
   - If LED stays on/off: Adjust potentiometer
   - Goal: LED blinks with each wheel pattern change
6. **Test**: Run vehicle and check pulse counts

### Optimal Settings
- **LED behavior**: Blinks only when pattern changes
- **Serial output**: Consistent pulse counts at constant speed
- **No false triggers**: LED doesn't blink randomly

## 🔋 Power Considerations

### Power Supply Options

| Method | Voltage | Current | Use Case |
|--------|---------|---------|----------|
| USB (Computer) | 5V | 500mA | Development/Testing |
| USB Power Bank | 5V | 1-2A | Portable operation |
| DC Jack | 7-12V | >500mA | Standalone use |
| VIN Pin | 7-12V | Regulated | External power |

### Power Consumption
- **Arduino Uno**: ~50mA (idle)
- **LM393 Sensor**: ~15mA
- **Total System**: ~70-100mA

## 🛡️ Safety Precautions

⚠️ **Important Safety Guidelines**

- ✅ Double-check all connections before powering on
- ✅ Use proper voltage (5V for sensor, 7-12V for Arduino VIN)
- ✅ Avoid short circuits
- ✅ Disconnect power when modifying connections
- ✅ Ensure sensor is securely mounted
- ❌ Don't apply reverse polarity
- ❌ Don't exceed 5.5V on sensor VCC
- ❌ Don't operate near water or moisture

## 🔍 Troubleshooting Hardware

### LED Not Lighting
- **Check**: Power connections (VCC, GND)
- **Verify**: Arduino is powered (onboard LED should be on)
- **Test**: Measure voltage at sensor VCC pin (should be ~5V)

### No Pulse Detection
- **Adjust**: Sensor sensitivity with potentiometer
- **Check**: Sensor-to-wheel distance (2-5mm optimal)
- **Verify**: Wheel surface has contrast/pattern
- **Test**: Move object in front of sensor manually

### Erratic Readings
- **Secure**: Loose connections or mounting
- **Reduce**: Ambient light interference
- **Clean**: Sensor lens and wheel surface
- **Adjust**: Sensitivity (less sensitive if over-triggering)

## 📸 Assembly Photos

> **Note**: Add your actual assembly photos here when available

Recommended photos to include:
1. Complete system overview
2. Close-up of sensor module
3. Sensor mounting position
4. Arduino connections
5. Working system in action

## 📐 Mechanical Specifications

### Sensor Module Dimensions
- **Length**: ~32mm
- **Width**: ~14mm
- **Height**: ~10mm (with components)
- **Mounting holes**: 3mm diameter (if available)

### Recommended Mounting Hardware
- M3 screws and nuts
- Zip ties for quick assembly
- Double-sided tape (for temporary testing)
- 3D printed brackets (custom fit)

## 🔗 Additional Resources

- [Arduino Uno Pinout](https://docs.arduino.cc/hardware/uno-rev3)
- [LM393 Datasheet](https://www.ti.com/lit/ds/symlink/lm393.pdf)
- [PiRacer Documentation](https://github.com/SEA-ME/PiRacer_Autonomous_Driving)

---

*Last Updated: February 12, 2026*
