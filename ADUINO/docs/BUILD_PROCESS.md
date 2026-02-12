# Build Process Documentation

## 📚 Overview

This document explains how Arduino/PlatformIO code is compiled and uploaded to the microcontroller. Understanding this process helps with debugging, optimization, and advanced development.

## 🔄 Complete Build Pipeline

```
┌─────────────────────────────────────────────┐
│  1. Source Files                            │
│     - src/main.cpp (your code)              │
│     - Arduino libraries (.cpp/.c/.S)        │
└─────────────────┬───────────────────────────┘
                  ↓
┌─────────────────────────────────────────────┐
│  2. Compiler Frontend (avr-g++/avr-gcc)     │
│     - Preprocessing (#include, #define)     │
│     - Parsing (syntax analysis)             │
│     - IR generation (intermediate code)     │
└─────────────────┬───────────────────────────┘
                  ↓
┌─────────────────────────────────────────────┐
│  3. Compiler Backend (AVR Backend)          │
│     - AVR assembly generation               │
│     - Optimization (-Os)                    │
└─────────────────┬───────────────────────────┘
                  ↓
┌─────────────────────────────────────────────┐
│  4. Assembler                               │
│     - .o object file generation             │
│     - Machine code                          │
└─────────────────┬───────────────────────────┘
                  ↓
┌─────────────────────────────────────────────┐
│  5. Archiver (avr-gcc-ar)                   │
│     - libFrameworkArduino.a creation        │
│     - Static library bundling               │
└─────────────────┬───────────────────────────┘
                  ↓
┌─────────────────────────────────────────────┐
│  6. Linker (avr-g++ with linker)            │
│     - Combine all .o files                  │
│     - Link libraries                        │
│     - firmware.elf generation               │
└─────────────────┬───────────────────────────┘
                  ↓
┌─────────────────────────────────────────────┐
│  7. objcopy (Format conversion)             │
│     - ELF → HEX                             │
│     - firmware.hex                          │
└─────────────────┬───────────────────────────┘
                  ↓
┌─────────────────────────────────────────────┐
│  8. Upload (avrdude)                        │
│     - Transfer to Arduino via USB           │
└─────────────────────────────────────────────┘
```

---

## 📝 Detailed Stage Breakdown

### Stage 1: Source Files

**Input**: Your C++ code and Arduino libraries

```cpp
// main.cpp
#include <Arduino.h>

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Your code
}
```

**What happens**:
- Collects all `.cpp`, `.c`, and `.S` files
- Includes Arduino core libraries
- Gathers header files (`.h`)

**Key files in this project**:
```
src/main.cpp
~/.platformio/packages/framework-arduino-avr/cores/arduino/*.cpp
~/.platformio/packages/framework-arduino-avr/cores/arduino/*.c
```

---

### Stage 2: Compiler Frontend (Preprocessing & Parsing)

**Tool**: `avr-g++` / `avr-gcc`

**Step 2.1: Preprocessing**
```cpp
// Before preprocessing
#include <Arduino.h>
#define LED_PIN 13

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

// After preprocessing (simplified)
// [All Arduino.h contents inserted here]
void setup() {
  pinMode(13, OUTPUT);  // LED_PIN replaced with 13
}
```

**Actions**:
- Resolves `#include` directives (inserts header contents)
- Expands `#define` macros
- Processes conditional compilation (`#ifdef`, `#ifndef`)
- Removes comments

**Step 2.2: Parsing & IR Generation**
- Checks C++ syntax
- Builds Abstract Syntax Tree (AST)
- Generates platform-independent Intermediate Representation (IR)
- Performs semantic analysis

**Example command**:
```bash
avr-g++ -c -g -Os -Wall -std=gnu++11 -fpermissive -fno-exceptions \
  -ffunction-sections -fdata-sections -fno-threadsafe-statics \
  -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10819 \
  -I"include" -I"src" main.cpp -o main.cpp.o
```

---

### Stage 3: Compiler Backend (Code Generation)

**Tool**: AVR Backend of GCC

**What happens**:
- Converts IR to AVR assembly language
- Applies optimizations (specified by `-Os` flag)
- Generates architecture-specific code for ATmega328P

**Optimization flags explained**:
- `-Os`: Optimize for size (important for limited flash memory)
- `-O0`: No optimization (for debugging)
- `-O1`: Basic optimization
- `-O2`: More optimization
- `-O3`: Maximum optimization (may increase code size)

**Assembly example** (simplified):
```assembly
; C code: digitalWrite(13, HIGH);
; Assembly output:
sbi 0x05, 5      ; Set bit in I/O register
```

---

### Stage 4: Assembler

**Tool**: `avr-as` (AVR Assembler)

**What happens**:
- Converts assembly code to machine code (binary)
- Creates object files (`.o`) for each source file
- Machine code is specific to ATmega328P instruction set

**Output**:
```
.pio/build/uno/src/main.cpp.o           (Your code)
.pio/build/uno/FrameworkArduino/*.o     (Arduino core)
```

**Object file contents**:
- Machine code (binary instructions)
- Symbol table (function names, addresses)
- Relocation information (for linker)

---

### Stage 5: Archiver

**Tool**: `avr-gcc-ar` (AVR Archiver)

**What happens**:
- Bundles Arduino framework object files
- Creates static library: `libFrameworkArduino.a`
- Allows for efficient reuse across projects

**Command**:
```bash
avr-gcc-ar rcs libFrameworkArduino.a \
  wiring_digital.c.o \
  wiring_analog.c.o \
  HardwareSerial.cpp.o \
  [... many more .o files ...]
```

**Why archive?**:
- Faster compilation (Arduino core only compiled once)
- Organized library management
- Selective linking (only used functions included)

---

### Stage 6: Linker

**Tool**: `avr-g++` with linker (`ld`)

**What happens**:
- Combines all `.o` object files
- Links with `libFrameworkArduino.a`
- Resolves symbols (connects function calls to definitions)
- Determines final memory addresses
- Creates executable: `firmware.elf`

**Command**:
```bash
avr-g++ -Os -mmcu=atmega328p -Wl,--gc-sections \
  main.cpp.o libFrameworkArduino.a \
  -o firmware.elf -lm
```

**Linker flags**:
- `-Wl,--gc-sections`: Remove unused code (saves memory)
- `-lm`: Link math library

**Memory layout decided here**:
```
Flash Memory (32KB):
├─ Bootloader (0.5KB)
├─ Your program code
└─ Arduino libraries

RAM (2KB):
├─ Global variables
├─ Stack (grows down)
└─ Heap (grows up)
```

**ELF file contents**:
- Complete program binary
- Debug information
- Symbol table
- Section information (text, data, bss)

---

### Stage 7: Format Conversion (objcopy)

**Tool**: `avr-objcopy`

**What happens**:
- Converts ELF format to Intel HEX format
- Removes debug information
- Creates uploadable file: `firmware.hex`

**Command**:
```bash
avr-objcopy -O ihex -R .eeprom firmware.elf firmware.hex
```

**Why HEX format?**:
- Standard format for microcontroller programming
- Human-readable ASCII representation
- Contains address and data information
- Includes checksums for verification

**HEX file example**:
```
:100000000C9435000C945D000C945D000C945D0024
:100010000C945D000C945D000C945D000C945D00EC
:10002000C945D000C945D000C945D000C945D00DC
...
```

**Format breakdown**:
```
:10 0000 00 [data...] [checksum]
 │    │   │      │         └─ Verification checksum
 │    │   │      └─────────── Data bytes
 │    │   └────────────────── Record type (00 = data)
 │    └────────────────────── Address
 └─────────────────────────── Byte count
```

---

### Stage 8: Upload (avrdude)

**Tool**: `avrdude` (AVR Downloader Uploader)

**What happens**:
- Communicates with Arduino bootloader via USB
- Transfers `firmware.hex` to flash memory
- Verifies successful upload
- Resets Arduino to run new program

**Command**:
```bash
avrdude -v -p atmega328p -c arduino -P /dev/cu.usbserial \
  -b 115200 -D -U flash:w:firmware.hex:i
```

**Flag explanations**:
- `-p atmega328p`: Target microcontroller
- `-c arduino`: Programmer type (Arduino bootloader)
- `-P /dev/cu.usbserial`: Serial port
- `-b 115200`: Baud rate
- `-D`: Don't erase before programming
- `-U flash:w:firmware.hex:i`: Write HEX to flash

**Upload process**:
1. Reset Arduino (activates bootloader)
2. Establish communication
3. Erase flash memory (if needed)
4. Write program data
5. Verify written data
6. Exit bootloader
7. Arduino resets and runs your program

---

## 🔍 Verification & Analysis

### Check Compiled Sizes

```bash
# After building
pio run

# Output example:
# RAM:   [=         ]   9.8% (used 200 bytes from 2048 bytes)
# Flash: [==        ]  10.2% (used 3296 bytes from 32256 bytes)
```

### View Detailed Memory Usage

```bash
avr-size -C --mcu=atmega328p .pio/build/uno/firmware.elf

# Output:
# AVR Memory Usage
# Device: atmega328p
# 
# Program:    3296 bytes (10.2% Full)
# Data:        200 bytes (9.8% Full)
```

### Examine Symbol Table

```bash
avr-nm .pio/build/uno/firmware.elf

# Shows all functions and their addresses
# Example output:
# 00000080 T setup
# 00000094 T loop
# 000000b2 T ISR_countPulse
```

### Generate Assembly Listing

```bash
avr-objdump -d .pio/build/uno/firmware.elf > disassembly.txt

# Creates human-readable assembly listing
```

---

## 🎯 PlatformIO Build Commands

### Basic Commands

```bash
# Clean previous builds
pio run -t clean

# Compile only (don't upload)
pio run

# Compile and upload
pio run -t upload

# Upload only (if already compiled)
pio run -t upload --upload-port /dev/cu.usbserial

# Verbose output (see all commands)
pio run -v

# Clean, then build
pio run -t clean && pio run
```

### Advanced Commands

```bash
# Build for specific environment
pio run -e uno

# Show build size
pio run -t size

# Generate compilation database (for IDE)
pio run -t compiledb

# Monitor serial port after upload
pio run -t upload && pio device monitor
```

### Build Flags Customization

In `platformio.ini`:
```ini
[env:uno]
platform = atmelavr
board = uno
framework = arduino

# Optimization levels
build_flags = 
    -Os              ; Optimize for size (default)
    ; -O0            ; No optimization (for debugging)
    ; -O2            ; Optimize for speed
    
    # Enable all warnings
    -Wall
    -Wextra
    
    # Define custom macros
    -DDEBUG_MODE=1
    -DVERSION=1.0.0
```

---

## 🐛 Debugging Build Issues

### Common Errors

**Error: "Undefined reference to..."**
```
Solution: Missing library or function implementation
Check: #include statements and library dependencies
```

**Error: "Section `.text' will not fit in region `text'"**
```
Solution: Program too large for flash memory
Fix: Enable optimizations (-Os), remove unused code
```

**Error: "Not enough memory"**
```
Solution: Too many global variables
Fix: Use PROGMEM for constants, optimize data structures
```

**Error: "Multiple definitions of..."**
```
Solution: Function defined in multiple files
Fix: Use header guards, check for duplicate definitions
```

### Build Process Troubleshooting

```bash
# Verbose compilation
pio run -v 2>&1 | tee build.log

# Check intermediate files
ls -lh .pio/build/uno/

# Verify toolchain versions
pio system info
```

---

## 📊 Build Performance

### Optimization Comparison

| Flag | Code Size | RAM Usage | Speed | Build Time |
|------|-----------|-----------|-------|------------|
| `-O0` | 100% | 100% | Slow | Fast |
| `-Os` | 70% | 85% | Medium | Medium |
| `-O2` | 85% | 90% | Fast | Slow |
| `-O3` | 95% | 95% | Fastest | Slowest |

**Recommendation**: Use `-Os` (default) for Arduino projects

### Build Time Optimization

**First build**: ~15-30 seconds (compiles Arduino core)  
**Incremental build**: ~2-5 seconds (only changed files)

**Tips**:
- Use `ccache` for faster compilation
- Don't clean unless necessary
- Close unnecessary programs during build

---

## 🔗 Additional Resources

- [GCC AVR Options](https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html)
- [AVRDUDE Documentation](https://www.nongnu.org/avrdude/user-manual/avrdude.html)
- [Intel HEX Format](https://en.wikipedia.org/wiki/Intel_HEX)
- [PlatformIO Build System](https://docs.platformio.org/en/latest/projectconf/section_env_build.html)
- [Arduino Build Process](https://github.com/arduino/Arduino/wiki/Build-Process)

---

*Last Updated: February 12, 2026*
