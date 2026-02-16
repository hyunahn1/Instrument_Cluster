# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### In Progress
- Qt C++ implementation
- Basic UI widgets

---

## [0.5.0] - 2026-02-16

### Added
- Complete project documentation
  - Hardware analysis
  - Feature specification
  - Implementation plan
  - Verification plan
- Python bridge for PiRacer data
  - Battery monitoring (INA219)
  - Direction tracking
  - JSON output format
- Qt project structure
  - CMakeLists.txt and dashboard.pro
  - Main source files
  - Widget implementations
  - Serial communication
  - Data processing utilities

### Features
- SpeedometerWidget: Analog gauge with digital display
- RpmGauge: Semi-circle RPM gauge
- BatteryWidget: Color-coded battery status
- SerialReader: Arduino communication with auto-reconnect
- DataProcessor: Pulse to km/h and RPM conversion
- CalibrationManager: JSON-based calibration

---

## [0.1.0] - 2026-02-16

### Added
- Initial project setup
- Documentation framework
- Python bridge prototype

---

**Next Steps**: Build and test on Raspberry Pi 4
