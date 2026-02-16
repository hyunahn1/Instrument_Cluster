#!/usr/bin/env python3
"""
PiRacer Bridge - Python to Qt Communication
============================================

PiRacer 차량 데이터(배터리, 전력 등)를 읽어서 
JSON 형식으로 stdout에 출력하여 Qt 애플리케이션과 통신합니다.

Author: Ahn Hyunjun
Date: 2026-02-16
Version: 1.0.0
"""

import json
import sys
import time
import struct
from collections import deque
from statistics import median
from typing import Dict, Optional

try:
    from piracer.vehicles import PiRacerStandard, PiRacerPro
    PIRACER_AVAILABLE = True
except ImportError:
    PIRACER_AVAILABLE = False
    print("Warning: piracer-py not installed. Running in simulation mode.", file=sys.stderr)

try:
    import can
    CAN_AVAILABLE = True
except ImportError:
    CAN_AVAILABLE = False
    print("Warning: python-can not installed. CAN input disabled.", file=sys.stderr)


class BatteryMonitor:
    """
    Coulomb counting + EMA 스무딩 기반 배터리 SOC 추정기.
    """

    # 단일 셀 LiPo OCV 맵 (무부하시 근사)
    OCV_TABLE_CELL = [
        (4.20, 100.0),
        (4.12, 90.0),
        (4.04, 80.0),
        (3.98, 70.0),
        (3.92, 60.0),
        (3.86, 50.0),
        (3.80, 40.0),
        (3.74, 30.0),
        (3.68, 20.0),
        (3.55, 10.0),
        (3.30, 0.0),
    ]

    def __init__(self, capacity_mah: float = 2500.0, alpha: float = 0.18):
        self.capacity_mah = max(100.0, capacity_mah)
        self.alpha = min(1.0, max(0.01, alpha))
        self.slew_up_per_sec = 2.0
        self.slew_down_per_sec = 3.0
        self.last_update_time = time.time()
        self.last_voltage = 0.0
        self.soc_percent = 100.0
        self.is_initialized = False
        self.voltage_history = deque(maxlen=7)

    def _detect_cell_count(self, voltage: float) -> int:
        # 현장 배터리(2S/3S) 자동 추정
        if voltage >= 10.0:
            return 3
        if voltage >= 6.0:
            return 2
        return 1

    def estimate_soc_from_voltage(self, voltage: float) -> float:
        cell_count = self._detect_cell_count(voltage)
        per_cell_voltage = voltage / float(cell_count)

        for idx in range(len(self.OCV_TABLE_CELL) - 1):
            v_high, soc_high = self.OCV_TABLE_CELL[idx]
            v_low, soc_low = self.OCV_TABLE_CELL[idx + 1]
            if per_cell_voltage >= v_high:
                return soc_high
            if v_low <= per_cell_voltage < v_high:
                ratio = (per_cell_voltage - v_low) / (v_high - v_low)
                return soc_low + ratio * (soc_high - soc_low)
        return 0.0

    def update(self, voltage: float, current_ma: float) -> float:
        now = time.time()
        # Remove short spikes first.
        self.voltage_history.append(voltage)
        stable_voltage = float(median(self.voltage_history))

        if not self.is_initialized:
            self.soc_percent = self.estimate_soc_from_voltage(stable_voltage)
            self.last_update_time = now
            self.last_voltage = stable_voltage
            self.is_initialized = True
            return self.soc_percent

        dt = max(0.05, now - self.last_update_time)
        self.last_update_time = now

        # Use OCV-based SOC + EMA smoothing (more stable for dashboard).
        target = max(0.0, min(100.0, self.estimate_soc_from_voltage(stable_voltage)))
        prev = self.soc_percent
        smoothed = self.alpha * target + (1.0 - self.alpha) * prev

        # Limit sudden jumps so value changes smoothly.
        delta = smoothed - prev
        max_up = self.slew_up_per_sec * dt
        max_down = self.slew_down_per_sec * dt
        if delta > max_up:
            smoothed = prev + max_up
        elif delta < -max_down:
            smoothed = prev - max_down

        self.soc_percent = smoothed
        self.last_voltage = stable_voltage
        return self.soc_percent


class PiRacerBridge:
    """
    PiRacer 데이터를 읽어서 JSON으로 출력하는 브릿지 클래스
    """
    
    # 배터리 전압 범위 (LiPo 2S 기준)
    BATTERY_V_MIN = 6.4   # 0% (방전 하한)
    BATTERY_V_MAX = 8.4   # 100% (완충)
    
    # 업데이트 주기 (초)
    UPDATE_INTERVAL = 0.5
    # 현재 현장 로그 기준 속도 CAN ID (candump: "can0 123 [8] ..")
    SPEED_CAN_ID = 0x123
    
    def __init__(self, vehicle_type: str = "standard"):
        """
        PiRacer 브릿지 초기화
        
        Args:
            vehicle_type: "standard" 또는 "pro"
        """
        self.vehicle_type = vehicle_type
        self.piracer: Optional[PiRacerStandard] = None
        self.last_throttle = 0.0  # 마지막 스로틀 값 추적 (방향 감지용)
        self.can_bus = None
        self.last_can_speed_kmh = 0.0
        self.last_can_rpm = 0.0
        self.battery_monitor = BatteryMonitor(capacity_mah=2500.0, alpha=0.12)
        
        if PIRACER_AVAILABLE:
            try:
                if vehicle_type == "pro":
                    self.piracer = PiRacerPro()
                else:
                    self.piracer = PiRacerStandard()
                print("PiRacer initialized successfully!", file=sys.stderr)
            except Exception as e:
                print(f"Error initializing PiRacer: {e}", file=sys.stderr)
                self.piracer = None
        else:
            print("Running in simulation mode", file=sys.stderr)

        if CAN_AVAILABLE:
            try:
                self.can_bus = can.interface.Bus(channel="can0", bustype="socketcan")
                print("CAN bus initialized on can0", file=sys.stderr)
            except Exception as e:
                print(f"Warning: failed to open can0: {e}", file=sys.stderr)
                self.can_bus = None
    
    def calculate_battery_percent(self, voltage: float) -> float:
        """
        배터리 전압을 퍼센트로 변환
        
        Args:
            voltage: 배터리 전압 (V)
        
        Returns:
            배터리 퍼센트 (0.0 ~ 100.0)
        """
        percent = ((voltage - self.BATTERY_V_MIN) / 
                   (self.BATTERY_V_MAX - self.BATTERY_V_MIN)) * 100.0
        # 0-100 범위로 제한
        return max(0.0, min(100.0, percent))

    def read_can_data(self) -> Dict:
        result: Dict = {}
        if self.can_bus is None:
            return result

        # 한 주기마다 버스에서 최대 10개까지 비차단으로 읽음
        for _ in range(10):
            try:
                msg = self.can_bus.recv(timeout=0.0)
            except Exception:
                break

            if msg is None:
                break

            if msg.arbitration_id == self.SPEED_CAN_ID and len(msg.data) >= 1:
                # 포맷 1) 1바이트 정수 속도(km/h) - 현재 candump 포맷
                # 예: 11 00 00 00 ... -> 17 km/h
                if len(msg.data) >= 4 and msg.data[1] == 0 and msg.data[2] == 0 and msg.data[3] == 0:
                    self.last_can_speed_kmh = float(int(msg.data[0]))
                    result["speed_kmh"] = self.last_can_speed_kmh
                elif len(msg.data) >= 4:
                    # 포맷 2) 4바이트 float 속도(km/h)
                    try:
                        self.last_can_speed_kmh = float(struct.unpack("<f", msg.data[0:4])[0])
                        result["speed_kmh"] = self.last_can_speed_kmh
                    except struct.error:
                        pass

            elif msg.arbitration_id == (self.SPEED_CAN_ID + 1) and len(msg.data) >= 4:
                try:
                    self.last_can_rpm = float(struct.unpack("<f", msg.data[0:4])[0])
                    result["rpm"] = self.last_can_rpm
                except struct.error:
                    pass

        return result
    
    def get_direction_from_throttle(self, throttle: float) -> str:
        """
        스로틀 값으로 방향 결정
        
        Args:
            throttle: 스로틀 값 (-1.0 ~ 1.0)
        
        Returns:
            "F" (Forward), "R" (Reverse), 또는 "N" (Neutral)
        """
        if throttle > 0.05:  # Dead zone 고려
            return "F"
        elif throttle < -0.05:
            return "R"
        else:
            return "N"
    
    def read_piracer_data(self) -> Dict:
        """
        PiRacer에서 실제 데이터 읽기
        
        Returns:
            데이터 딕셔너리
        """
        can_data = self.read_can_data()
        
        try:
            if self.piracer is not None:
                voltage = self.piracer.get_battery_voltage()
                current = self.piracer.get_battery_current()
                power = self.piracer.get_power_consumption()
            else:
                sim = self.get_simulation_data()
                return sim

            percent = self.battery_monitor.update(voltage, current)
            direction = self.get_direction_from_throttle(self.last_throttle)
            if "speed_kmh" in can_data and abs(float(can_data["speed_kmh"])) > 0.1 and direction == "N":
                direction = "F"

            output = {
                "battery": {
                    "voltage": round(voltage, 2),
                    "percent": round(percent, 1),
                    "current": round(current, 1),
                    "power": round(power, 2)
                },
                "direction": direction,
                "timestamp": time.time()
            }
            if "speed_kmh" in can_data:
                output["speed_kmh"] = round(float(can_data["speed_kmh"]), 2)
            if "rpm" in can_data:
                output["rpm"] = round(float(can_data["rpm"]), 1)
            return output
        except Exception as e:
            print(f"Error reading PiRacer data: {e}", file=sys.stderr)
            return self.get_simulation_data()
    
    def get_simulation_data(self) -> Dict:
        """
        시뮬레이션 데이터 생성 (테스트용)
        
        Returns:
            시뮬레이션 데이터 딕셔너리
        """
        # 시간에 따라 변화하는 시뮬레이션 데이터
        t = time.time()
        voltage = 7.8 + 0.2 * (t % 10) / 10  # 7.8 ~ 8.0V 변동
        percent = self.calculate_battery_percent(voltage)
        
        return {
            "battery": {
                "voltage": round(voltage, 2),
                "percent": round(percent, 1),
                "current": round(150.0 + 50.0 * (t % 5) / 5, 1),  # 150~200 mA
                "power": round(voltage * 0.15, 2)  # W
            },
            "direction": "F",  # 항상 전진
            "timestamp": t
        }
    
    def run(self):
        """
        메인 루프: 데이터를 읽고 JSON으로 출력
        """
        print("PiRacer Bridge started", file=sys.stderr)
        print(f"Update interval: {self.UPDATE_INTERVAL}s", file=sys.stderr)
        print(f"Vehicle type: {self.vehicle_type}", file=sys.stderr)
        print("-" * 50, file=sys.stderr)
        
        try:
            while True:
                data = self.read_piracer_data()
                
                # JSON 형식으로 stdout에 출력 (Qt가 읽음)
                json_output = json.dumps(data)
                print(json_output)
                sys.stdout.flush()  # 버퍼 즉시 플러시
                
                time.sleep(self.UPDATE_INTERVAL)
        
        except KeyboardInterrupt:
            print("\nPiRacer Bridge stopped by user", file=sys.stderr)
        except Exception as e:
            print(f"Fatal error: {e}", file=sys.stderr)
            sys.exit(1)


def main():
    """
    메인 함수
    """
    import argparse
    
    parser = argparse.ArgumentParser(description="PiRacer Bridge")
    parser.add_argument(
        "--type",
        choices=["standard", "pro"],
        default="standard",
        help="PiRacer vehicle type (default: standard)"
    )
    parser.add_argument(
        "--interval",
        type=float,
        default=0.5,
        help="Update interval in seconds (default: 0.5)"
    )
    
    args = parser.parse_args()
    
    bridge = PiRacerBridge(vehicle_type=args.type)
    bridge.UPDATE_INTERVAL = args.interval
    
    bridge.run()


if __name__ == "__main__":
    main()
