# API Documentation

## Code Structure Overview

This document describes the functions, variables, and configurations used in the PiRacer Speed Sensor System.

## Table of Contents
- [Configuration Macros](#configuration-macros)
- [Global Variables](#global-variables)
- [Functions](#functions)
- [Interrupt Service Routine](#interrupt-service-routine)
- [Serial Output Format](#serial-output-format)

---

## Configuration Macros

### `SPEED_SENSOR_PIN`
```cpp
#define SPEED_SENSOR_PIN 2
```
**Description**: Digital pin number for speed sensor connection  
**Type**: Integer  
**Value**: 2 (INT0 on Arduino Uno)  
**Note**: Must be an interrupt-capable pin (2 or 3 on Uno)

### `SERIAL_BAUD_RATE`
```cpp
#define SERIAL_BAUD_RATE 9600
```
**Description**: Serial communication baud rate  
**Type**: Integer  
**Value**: 9600 bits per second  
**Note**: Match this in your serial monitor

### `UPDATE_INTERVAL_MS`
```cpp
#define UPDATE_INTERVAL_MS 500
```
**Description**: Speed calculation and display update interval  
**Type**: Integer  
**Unit**: Milliseconds  
**Value**: 500ms (0.5 seconds)  
**Range**: 100-5000ms recommended  
**Note**: Smaller values = more frequent updates but noisier data

---

## Global Variables

### `g_pulseCount`
```cpp
volatile unsigned long g_pulseCount = 0;
```
**Description**: Pulse counter incremented by ISR  
**Type**: Volatile unsigned long  
**Access**: Modified by ISR, read by main loop  
**Range**: 0 to 4,294,967,295  
**Note**: `volatile` keyword ensures proper ISR/main loop communication

### `g_lastUpdateTime`
```cpp
unsigned long g_lastUpdateTime = 0;
```
**Description**: Timestamp of last speed calculation  
**Type**: Unsigned long  
**Unit**: Milliseconds since boot  
**Used by**: `loop()` function for timing  
**Note**: Uses `millis()` which overflows after ~50 days

### `g_currentSpeed`
```cpp
float g_currentSpeed = 0.0;
```
**Description**: Most recently calculated speed value  
**Type**: Float  
**Unit**: Pulses per second  
**Precision**: 2 decimal places in output  
**Range**: 0.0 to theoretical max of sensor

---

## Functions

### `setup()`
```cpp
void setup()
```
**Description**: Initialization function called once at startup  
**Parameters**: None  
**Returns**: void  

**Actions performed**:
1. Initializes serial communication
2. Waits for serial port connection
3. Configures sensor pin as INPUT
4. Attaches interrupt handler
5. Prints startup banner
6. Initializes timestamp

**Usage**: Called automatically by Arduino framework

---

### `loop()`
```cpp
void loop()
```
**Description**: Main program loop, called repeatedly  
**Parameters**: None  
**Returns**: void  

**Actions performed**:
1. Checks if update interval has elapsed
2. Calls calculation functions if interval passed
3. Displays results
4. Resets counters for next cycle

**Timing**: Executes continuously, updates every `UPDATE_INTERVAL_MS`

---

### `calculateSpeed()`
```cpp
void calculateSpeed()
```
**Description**: Calculates speed from pulse count  
**Parameters**: None  
**Returns**: void  
**Updates**: `g_currentSpeed`

**Formula**:
```
Speed (pulse/s) = Pulse Count / Interval (seconds)
                = g_pulseCount / (UPDATE_INTERVAL_MS / 1000.0)
```

**Example**:
```
Pulse Count = 12
Interval = 500ms = 0.5s
Speed = 12 / 0.5 = 24.0 pulse/s
```

---

### `displaySpeed()`
```cpp
void displaySpeed()
```
**Description**: Outputs speed data to serial monitor  
**Parameters**: None  
**Returns**: void  
**Output**: Formatted string with pulse count, speed, and timestamp

**Output Format**:
```
Pulses: [count] | Speed: [value] pulse/s | Time: [seconds] s
```

**Example**:
```
Pulses: 12 | Speed: 24.00 pulse/s | Time: 2.50 s
```

---

### `resetCounters()`
```cpp
void resetCounters()
```
**Description**: Resets pulse counter for next measurement cycle  
**Parameters**: None  
**Returns**: void  
**Action**: Sets `g_pulseCount` to 0

**Note**: Called after each display update to start fresh count

---

### `printStartupBanner()`
```cpp
void printStartupBanner()
```
**Description**: Prints system information banner at startup  
**Parameters**: None  
**Returns**: void  

**Output**:
```
=========================================
   PiRacer Speed Sensor System v1.0     
=========================================
Sensor: LM393 IR Speed Sensor
Platform: Arduino Uno (ATmega328P)
Update Interval: 500 ms
=========================================
Starting measurements...
```

**Note**: Uses `F()` macro for PROGMEM storage to save RAM

---

## Interrupt Service Routine

### `ISR_countPulse()`
```cpp
void ISR_countPulse()
```
**Description**: Interrupt handler called on sensor pulse detection  
**Trigger**: FALLING edge (HIGH to LOW transition)  
**Parameters**: None  
**Returns**: void  
**Action**: Increments `g_pulseCount`

**Important characteristics**:
- ⚡ Executes immediately when triggered
- 🚀 Must be fast (no delays or serial prints)
- 🔒 Uses `volatile` variables
- 🎯 Triggered by hardware interrupt

**Trigger Setup**:
```cpp
attachInterrupt(digitalPinToInterrupt(SPEED_SENSOR_PIN), 
                ISR_countPulse, 
                FALLING);
```

**Timing**: Typical execution time < 5 microseconds

---

## Serial Output Format

### Message Types

#### Startup Messages
```
=========================================
   PiRacer Speed Sensor System v1.0     
=========================================
Sensor: LM393 IR Speed Sensor
Platform: Arduino Uno (ATmega328P)
Update Interval: 500 ms
=========================================
Starting measurements...
```

#### Data Output (repeated every UPDATE_INTERVAL_MS)
```
Pulses: [uint] | Speed: [float] pulse/s | Time: [float] s
```

**Field descriptions**:
- **Pulses**: Raw pulse count in interval (unsigned integer)
- **Speed**: Calculated speed (float, 2 decimal places)
- **Time**: Elapsed time since boot (float, 2 decimal places)

### Parsing Serial Output

#### Example Parser (Python)
```python
import serial

ser = serial.Serial('/dev/ttyUSB0', 9600)

while True:
    line = ser.readline().decode('utf-8').strip()
    if "Pulses:" in line:
        parts = line.split('|')
        pulses = int(parts[0].split(':')[1].strip())
        speed = float(parts[1].split(':')[1].replace('pulse/s', '').strip())
        time = float(parts[2].split(':')[1].replace('s', '').strip())
        print(f"Speed: {speed} pulse/s")
```

---

## Customization Guide

### Change Update Interval
```cpp
// Faster updates (200ms)
#define UPDATE_INTERVAL_MS 200

// Slower updates (1 second)
#define UPDATE_INTERVAL_MS 1000
```

### Change Serial Baud Rate
```cpp
// Update both locations:
#define SERIAL_BAUD_RATE 115200
// And in platformio.ini:
monitor_speed = 115200
```

### Add Speed Unit Conversion
```cpp
void calculateSpeed() {
  float intervalSeconds = UPDATE_INTERVAL_MS / 1000.0;
  g_currentSpeed = g_pulseCount / intervalSeconds;
  
  // Add conversion (example: assume 20 pulses = 1 meter)
  float speedMetersPerSecond = g_currentSpeed / 20.0;
  float speedKmPerHour = speedMetersPerSecond * 3.6;
}
```

### Change Interrupt Trigger Mode
```cpp
// Rising edge (LOW to HIGH)
attachInterrupt(digitalPinToInterrupt(SPEED_SENSOR_PIN), 
                ISR_countPulse, 
                RISING);

// Any change (HIGH↔LOW)
attachInterrupt(digitalPinToInterrupt(SPEED_SENSOR_PIN), 
                ISR_countPulse, 
                CHANGE);
```

---

## Memory Usage

### RAM Usage Breakdown
```
Global Variables:
- g_pulseCount (4 bytes)
- g_lastUpdateTime (4 bytes)
- g_currentSpeed (4 bytes)
- Serial buffers (~128 bytes)
Total: ~140-200 bytes of 2048 bytes (7-10%)
```

### Flash Memory Usage
```
Code: ~3KB of 32KB (9%)
Includes:
- Arduino core libraries
- Serial communication code
- Main program logic
```

---

## Performance Characteristics

### Timing Specifications
- **Interrupt latency**: < 5 μs
- **Max pulse rate**: ~4000 pulses/second (theoretical)
- **Update frequency**: 2 Hz (configurable)
- **Serial output rate**: ~40 bytes every 500ms

### Accuracy Considerations
- **Timer resolution**: 1ms (`millis()` function)
- **Pulse counting**: No missed pulses (interrupt-based)
- **Speed calculation**: Limited by update interval

---

*Last Updated: February 12, 2026*
