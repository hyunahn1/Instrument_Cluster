#!/usr/bin/env python3
"""
PiRacer Bridge 테스트 스크립트
============================

piracer_bridge.py의 출력을 읽어서 검증합니다.

Usage:
    python3 test_bridge.py
"""

import json
import subprocess
import sys
import time

def test_bridge_output():
    """
    브릿지 출력을 읽고 검증
    """
    print("=" * 60)
    print("PiRacer Bridge Test")
    print("=" * 60)
    print("Starting piracer_bridge.py...")
    print("Press Ctrl+C to stop\n")
    
    try:
        # piracer_bridge.py 실행
        process = subprocess.Popen(
            ["python3", "piracer_bridge.py"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1
        )
        
        line_count = 0
        valid_count = 0
        
        # stdout에서 한 줄씩 읽기
        for line in process.stdout:
            line = line.strip()
            if not line:
                continue
            
            line_count += 1
            
            try:
                # JSON 파싱
                data = json.loads(line)
                valid_count += 1
                
                # 데이터 추출
                battery = data.get("battery", {})
                voltage = battery.get("voltage", 0.0)
                percent = battery.get("percent", 0.0)
                current = battery.get("current", 0.0)
                power = battery.get("power", 0.0)
                direction = data.get("direction", "?")
                timestamp = data.get("timestamp", 0.0)
                
                # 화면에 출력
                print(f"\r[{line_count:4d}] "
                      f"Battery: {voltage:.2f}V ({percent:.1f}%) | "
                      f"Current: {current:.1f}mA | "
                      f"Power: {power:.2f}W | "
                      f"Dir: {direction}",
                      end="", flush=True)
                
                # 검증
                assert 0.0 <= voltage <= 10.0, f"Invalid voltage: {voltage}"
                assert 0.0 <= percent <= 100.0, f"Invalid percent: {percent}"
                assert direction in ["F", "R", "N"], f"Invalid direction: {direction}"
                
            except json.JSONDecodeError as e:
                print(f"\n[ERROR] Invalid JSON: {line}")
                print(f"  Error: {e}")
            except AssertionError as e:
                print(f"\n[ERROR] Validation failed: {e}")
            except Exception as e:
                print(f"\n[ERROR] Unexpected error: {e}")
        
    except KeyboardInterrupt:
        print("\n\n" + "=" * 60)
        print(f"Test stopped by user")
        print(f"Total lines: {line_count}")
        print(f"Valid JSON: {valid_count}")
        print(f"Success rate: {valid_count/line_count*100:.1f}%" if line_count > 0 else "N/A")
        print("=" * 60)
    except Exception as e:
        print(f"\n[FATAL ERROR] {e}")
        sys.exit(1)
    finally:
        if 'process' in locals():
            process.terminate()
            process.wait()

def test_import():
    """
    piracer 모듈 임포트 테스트
    """
    print("Testing piracer-py import...")
    try:
        from piracer.vehicles import PiRacerStandard
        print("✓ piracer-py is installed")
        return True
    except ImportError:
        print("✗ piracer-py is NOT installed (will run in simulation mode)")
        return False

if __name__ == "__main__":
    print("\n" + "=" * 60)
    print("PiRacer Bridge Test Suite")
    print("=" * 60 + "\n")
    
    # 1. 임포트 테스트
    piracer_available = test_import()
    
    if not piracer_available:
        print("\nNote: Running in simulation mode")
        print("Install piracer-py for real data:")
        print("  pip3 install piracer-py\n")
    
    # 2. 브릿지 출력 테스트
    input("Press Enter to start bridge test...")
    test_bridge_output()
