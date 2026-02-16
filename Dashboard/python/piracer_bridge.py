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
from typing import Dict, Optional

try:
    from piracer.vehicles import PiRacerStandard, PiRacerPro
    PIRACER_AVAILABLE = True
except ImportError:
    PIRACER_AVAILABLE = False
    print("Warning: piracer-py not installed. Running in simulation mode.", file=sys.stderr)


class PiRacerBridge:
    """
    PiRacer 데이터를 읽어서 JSON으로 출력하는 브릿지 클래스
    """
    
    # 배터리 전압 범위 (LiPo 2S 기준)
    BATTERY_V_MIN = 6.4   # 0% (방전 하한)
    BATTERY_V_MAX = 8.4   # 100% (완충)
    
    # 업데이트 주기 (초)
    UPDATE_INTERVAL = 0.5
    
    def __init__(self, vehicle_type: str = "standard"):
        """
        PiRacer 브릿지 초기화
        
        Args:
            vehicle_type: "standard" 또는 "pro"
        """
        self.vehicle_type = vehicle_type
        self.piracer: Optional[PiRacerStandard] = None
        self.last_throttle = 0.0  # 마지막 스로틀 값 추적 (방향 감지용)
        
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
        if self.piracer is None:
            # 시뮬레이션 데이터 (테스트용)
            return self.get_simulation_data()
        
        try:
            voltage = self.piracer.get_battery_voltage()
            current = self.piracer.get_battery_current()
            power = self.piracer.get_power_consumption()
            
            percent = self.calculate_battery_percent(voltage)
            direction = self.get_direction_from_throttle(self.last_throttle)
            
            return {
                "battery": {
                    "voltage": round(voltage, 2),
                    "percent": round(percent, 1),
                    "current": round(current, 1),
                    "power": round(power, 2)
                },
                "direction": direction,
                "timestamp": time.time()
            }
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
