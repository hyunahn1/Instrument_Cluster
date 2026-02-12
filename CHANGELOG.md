# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.0.0] - 2026-02-12

### Added
- Comprehensive documentation suite
  - API documentation with function reference
  - Hardware setup guide with diagrams
  - Build process documentation
- Professional README with badges
- CHANGELOG for version tracking
- MIT License file

### Changed
- Refactored code with proper documentation comments
- Enhanced serial output with timestamps
- Improved function naming conventions (ISR prefix)
- Updated platformio.ini with detailed configuration

### Fixed
- Memory optimization using F() macro for strings
- Proper volatile keyword usage for ISR variables

---

## [0.3.0] - 2026-02-11

### Added
- Modular function structure
  - `calculateSpeed()` for speed computation
  - `displaySpeed()` for serial output
  - `resetCounters()` for cleanup
  - `printStartupBanner()` for system info
- Configuration macros for easy customization
- Detailed code comments and documentation
- Time stamp in serial output

### Changed
- Improved code readability with clear sections
- Better variable naming (g_ prefix for globals)
- Enhanced startup banner with system information

---

## [0.2.0] - 2026-02-10

### Added
- Interrupt-based pulse counting (FALLING edge)
- Real-time speed calculation (500ms intervals)
- Serial output formatting
- Basic PlatformIO configuration

### Changed
- Switched from polling to interrupt-driven architecture
- Improved timing accuracy with `millis()`
- Enhanced serial output readability

### Fixed
- Pulse counting accuracy issues
- Timing drift in speed calculation

---

## [0.1.0] - 2026-02-09

### Added
- Initial project setup with PlatformIO
- Basic LM393 sensor integration
- Simple pulse counting logic
- Serial communication at 9600 baud
- Pin 2 configuration for sensor input

### Features
- Basic speed sensor reading
- Digital input from LM393 DO pin
- Simple serial output

---

## [Unreleased]

### Planned for v1.1.0
- Speed unit conversion (pulses/s to km/h or m/s)
- Calibration function for wheel circumference
- Average speed calculation over multiple intervals

### Future Versions
- LCD/OLED display support (v1.2.0)
- SD card data logging (v1.3.0)
- Wireless communication - WiFi/Bluetooth (v2.0.0)
- ROS integration for autonomous navigation (v2.1.0)
- Multi-sensor support for differential measurement (v2.2.0)

---
