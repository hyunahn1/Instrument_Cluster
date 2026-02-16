# Python Bridge

Python script that delivers PiRacer data (battery, direction, etc.) to Qt application.

## Installation

```bash
cd python
pip3 install -r requirements.txt
```

## Usage

### Standalone Execution (Testing)

```bash
# Basic execution (PiRacer Standard)
python3 piracer_bridge.py

# Use PiRacer Pro
python3 piracer_bridge.py --type pro

# Change update interval (1 second)
python3 piracer_bridge.py --interval 1.0
```

### Integration with Qt Application

Qt application automatically executes this script.
No manual execution needed.

## Output Format

Output to stdout in JSON format:

```json
{
  "battery": {
    "voltage": 7.85,
    "percent": 82.5,
    "current": 156.3,
    "power": 1.23
  },
  "direction": "F",
  "timestamp": 1708070400.123
}
```

### Field Description

- `battery.voltage`: Battery voltage (V)
- `battery.percent`: Battery level (%)
- `battery.current`: Current (mA)
- `battery.power`: Power consumption (W)
- `direction`: Direction ("F" = Forward, "R" = Reverse, "N" = Neutral)
- `timestamp`: Unix timestamp

## Simulation Mode

Automatically runs in simulation mode in environments where piracer-py is not installed.
Generates and outputs test data.

## Troubleshooting

### piracer-py Installation Failed

```bash
# Install I2C tools
sudo apt install i2c-tools

# Enable I2C via raspi-config
sudo raspi-config
# Interface Options > I2C > Yes

# Reboot
sudo reboot
```

### Permission Error

```bash
# Add user to i2c group
sudo usermod -a -G i2c $USER

# Re-login
```

## PiRacer API Reference

Available methods:

```python
from piracer.vehicles import PiRacerStandard

piracer = PiRacerStandard()

# Battery information
voltage = piracer.get_battery_voltage()      # float (V)
current = piracer.get_battery_current()      # float (mA)
power = piracer.get_power_consumption()      # float (W)

# Display (OLED 128x32)
display = piracer.get_display()              # SSD1306_I2C

# Vehicle control (not used for reading)
piracer.set_throttle_percent(0.5)   # Forward
piracer.set_steering_percent(0.0)   # Steering
```

## License

MIT License
