# PiRacer 계기판 구현 계획서

## 📋 문서 정보
- **프로젝트명**: PiRacer Luxury Sports Dashboard
- **버전**: 1.0.0
- **작성일**: 2026-02-16
- **작성자**: Ahn Hyunjun
- **예상 기간**: 5-7일 (Phase 1 기준)

---

## 1. 프로젝트 구조

### 1.1 디렉토리 구조
```
Dashboard/
├── docs/                           # 문서
│   ├── HARDWARE_ANALYSIS.md       # 하드웨어 분석
│   ├── SPECIFICATION.md           # 기능 명세서
│   ├── IMPLEMENTATION_PLAN.md     # 구현 계획서 (본 문서)
│   └── VERIFICATION_PLAN.md       # 검증 계획서
│
├── src/                           # C++ 소스 코드
│   ├── main.cpp                   # Qt 애플리케이션 진입점
│   ├── MainWindow.h               # 메인 윈도우 헤더
│   ├── MainWindow.cpp             # 메인 윈도우 구현
│   ├── widgets/                   # 커스텀 위젯
│   │   ├── SpeedometerWidget.h
│   │   ├── SpeedometerWidget.cpp
│   │   ├── RpmGauge.h
│   │   ├── RpmGauge.cpp
│   │   ├── BatteryWidget.h
│   │   └── BatteryWidget.cpp
│   ├── serial/                    # 시리얼 통신
│   │   ├── SerialReader.h
│   │   └── SerialReader.cpp
│   └── utils/                     # 유틸리티
│       ├── DataProcessor.h
│       ├── DataProcessor.cpp
│       ├── CalibrationManager.h
│       └── CalibrationManager.cpp
│
├── python/                        # Python 브릿지
│   ├── piracer_bridge.py         # PiRacer 데이터 읽기
│   └── requirements.txt          # Python 의존성
│
├── resources/                     # 리소스 파일
│   ├── fonts/                    # 폰트
│   │   ├── Roboto-Bold.ttf
│   │   ├── Roboto-Regular.ttf
│   │   └── RobotoMono-Regular.ttf
│   ├── icons/                    # 아이콘
│   │   └── battery_icon.svg
│   └── dashboard.qrc             # Qt 리소스 파일
│
├── tests/                        # 테스트 코드
│   ├── test_serial.cpp
│   ├── test_widgets.cpp
│   └── test_data_processing.cpp
│
├── config/                       # 설정 파일
│   ├── calibration.json         # 캘리브레이션 값
│   └── settings.json            # 애플리케이션 설정
│
├── CMakeLists.txt               # CMake 빌드 설정
├── dashboard.pro                # Qt Creator 프로젝트 파일
├── README.md                    # 프로젝트 설명
├── CHANGELOG.md                 # 변경 이력
└── LICENSE                      # 라이선스

```

---

## 2. 기술 스택

### 2.1 개발 환경
- **OS**: Raspberry Pi OS (Debian 기반)
- **IDE**: Qt Creator / VS Code
- **빌드 시스템**: CMake 3.16+ 또는 qmake
- **버전 관리**: Git

### 2.2 프로그래밍 언어 및 프레임워크

| 언어/프레임워크 | 버전 | 용도 |
|----------------|------|------|
| **C++** | C++17 | 메인 GUI 애플리케이션 |
| **Qt** | Qt5.15+ / Qt6 | GUI 프레임워크 |
| **Python** | 3.9+ | PiRacer 라이브러리 연동 |
| **QSerialPort** | Qt 내장 | 시리얼 통신 |
| **QPainter** | Qt 내장 | 커스텀 위젯 그리기 |

### 2.3 외부 라이브러리

#### C++ 라이브러리
- **Qt5/Qt6 Core**: 기본 Qt 기능
- **Qt5/Qt6 Widgets**: GUI 위젯
- **Qt5/Qt6 SerialPort**: 시리얼 통신
- **Qt5/Qt6 Network**: (선택) 네트워크 기능

#### Python 라이브러리
```txt
piracer-py>=0.2.0    # PiRacer 공식 라이브러리
pyserial>=3.5        # 시리얼 통신 (백업용)
```

---

## 3. 단계별 구현 계획

### Phase 1: 기본 UI 구현 (Day 1-2)

#### Day 1: 프로젝트 설정 및 레이아웃
- [x] 프로젝트 문서 작성
  - [x] HARDWARE_ANALYSIS.md
  - [x] SPECIFICATION.md
  - [x] IMPLEMENTATION_PLAN.md
- [ ] Qt 프로젝트 생성
  - [ ] CMakeLists.txt 또는 .pro 파일 작성
  - [ ] 기본 디렉토리 구조 생성
- [ ] MainWindow 클래스 구현
  - [ ] 1200×400 고정 크기 윈도우
  - [ ] 검은색 배경 (#0A0E1A)
  - [ ] 3칸 레이아웃 (좌/중앙/우)
- [ ] 폰트 리소스 추가
  - [ ] Roboto Bold, Regular 다운로드
  - [ ] Roboto Mono 다운로드
  - [ ] .qrc 파일에 등록

#### Day 2: 정적 위젯 구현
- [ ] SpeedometerWidget 클래스
  - [ ] 원형 게이지 그리기 (QPainter)
  - [ ] 눈금 그리기 (0~30)
  - [ ] 정적 바늘 표시 (테스트용)
  - [ ] 중앙 숫자 라벨 (QLabel)
- [ ] 좌측/우측 패널 레이아웃
  - [ ] QLabel로 임시 텍스트 배치
  - [ ] 색상 및 폰트 적용
- [ ] 하단 정보바
  - [ ] 최고 속도 라벨
  - [ ] Reset QPushButton

**완료 기준:**
- 실행하면 1200×400 창에 정적 계기판 표시
- 모든 요소가 올바른 위치에 배치

---

### Phase 2: 시리얼 통신 (Day 3)

#### Day 3-1: SerialReader 클래스
- [ ] SerialReader.h/cpp 작성
  - [ ] QSerialPort 초기화
  - [ ] 포트 자동 감지 (/dev/ttyUSB*, /dev/ttyACM*)
  - [ ] 9600 baud, 8N1 설정
- [ ] 데이터 파싱
  - [ ] readyRead() 시그널 연결
  - [ ] 줄 단위로 읽기 (`\n` 구분)
  - [ ] 정규표현식으로 파싱:
    ```cpp
    QRegularExpression re("Speed: (\\d+\\.\\d+) pulse/s");
    ```
- [ ] 시그널/슬롯 설계
  ```cpp
  signals:
      void speedDataReceived(float pulsePerSec);
      void connectionStatusChanged(bool connected);
  ```

#### Day 3-2: 데이터 흐름 연결
- [ ] SerialReader를 MainWindow에 통합
- [ ] speedDataReceived → SpeedometerWidget 업데이트
- [ ] 콘솔 출력으로 데이터 확인
- [ ] 연결 실패 시 에러 메시지 표시

**완료 기준:**
- Arduino 연결 시 실시간 속도 데이터 수신
- 화면에 숫자로 표시 (아직 애니메이션 없음)

---

### Phase 3: 동적 애니메이션 (Day 4)

#### Day 4-1: 바늘 애니메이션
- [ ] QPropertyAnimation 사용
  ```cpp
  QPropertyAnimation *needleAnimation;
  needleAnimation->setDuration(200);
  needleAnimation->setEasingCurve(QEasingCurve::OutCubic);
  ```
- [ ] 속도 → 각도 변환
  ```cpp
  float angle = (speedKmh / 30.0) * 270.0;  // 0~30km/h → 0~270°
  ```
- [ ] paintEvent()에서 회전된 바늘 그리기

#### Day 4-2: 숫자 업데이트
- [ ] QLabel setText() 호출
- [ ] 소수점 1자리 형식: `QString::number(speed, 'f', 1)`
- [ ] 100ms 타이머로 부드러운 업데이트

**완료 기준:**
- 바늘이 부드럽게 회전
- 숫자가 실시간 업데이트
- 60 FPS 유지

---

### Phase 4: RPM 및 보조 위젯 (Day 5)

#### Day 5-1: DataProcessor 클래스
- [ ] 속도 변환 함수
  ```cpp
  float pulseToKmh(float pulsePerSec);
  float pulseToRPM(float pulsePerSec);
  ```
- [ ] 캘리브레이션 값 로드
  ```cpp
  CalibrationManager::loadFromFile("config/calibration.json");
  ```

#### Day 5-2: RpmGauge 위젯
- [ ] 세미 서클 게이지 (180도)
- [ ] 0~500 RPM 범위
- [ ] 중앙에 큰 숫자 표시
- [ ] 시안 블루 색상 적용

#### Day 5-3: 우측 패널
- [ ] 방향 표시 (F/R)
  - 현재는 항상 "F" 고정 (Phase 2에서 동적 구현)
- [ ] 주행 시간 타이머
  ```cpp
  QElapsedTimer elapsedTimer;
  elapsedTimer.start();
  int seconds = elapsedTimer.elapsed() / 1000;
  ```

**완료 기준:**
- RPM 게이지 동작
- 시간이 증가
- 모든 데이터가 동기화

---

### Phase 5: 배터리 및 Python 연동 (Day 6)

#### Day 6-1: Python 브릿지
- [ ] `piracer_bridge.py` 작성
  ```python
  from piracer.vehicles import PiRacerStandard
  import json
  import sys
  
  piracer = PiRacerStandard()
  
  while True:
      voltage = piracer.get_battery_voltage()
      percent = calculate_percent(voltage)
      data = {"voltage": voltage, "percent": percent}
      print(json.dumps(data))
      sys.stdout.flush()
      time.sleep(0.5)
  ```

#### Day 6-2: C++에서 Python 프로세스 실행
- [ ] QProcess로 Python 스크립트 실행
  ```cpp
  QProcess *pythonProcess = new QProcess(this);
  pythonProcess->start("python3", QStringList() << "python/piracer_bridge.py");
  ```
- [ ] stdout 읽기 및 JSON 파싱
  ```cpp
  connect(pythonProcess, &QProcess::readyReadStandardOutput, this, &MainWindow::onPythonData);
  ```

#### Day 6-3: BatteryWidget 구현
- [ ] 배터리 아이콘 그리기 (SVG 또는 QPainter)
- [ ] 퍼센트 숫자 표시
- [ ] 전압 표시 (회색, 작은 폰트)
- [ ] 색상 코드 적용 (0~20%: 빨강, 깜빡임)

**완료 기준:**
- 실시간 배터리 잔량 표시
- 색상이 잔량에 따라 변경
- 저전압 경고 애니메이션 작동

---

### Phase 6: 최고 속도 및 통계 (Day 7)

#### Day 7-1: 통계 계산
- [ ] 최고 속도 추적
  ```cpp
  if (currentSpeed > maxSpeed) {
      maxSpeed = currentSpeed;
      emit newRecordAchieved();  // 애니메이션 트리거
  }
  ```
- [ ] 주행 거리 적분
  ```cpp
  float deltaDistance = speedKmh * (deltaTime / 3600.0);  // km
  totalDistance += deltaDistance;
  ```

#### Day 7-2: Reset 버튼 구현
- [ ] 버튼 클릭 시그널 연결
  ```cpp
  connect(resetButton, &QPushButton::clicked, this, &MainWindow::onResetClicked);
  ```
- [ ] maxSpeed 초기화
- [ ] 확인 플래시 애니메이션

#### Day 7-3: 신기록 애니메이션
- [ ] QPropertyAnimation으로 깜빡임
- [ ] 500ms on/off × 3회

**완료 기준:**
- 최고 속도가 정확히 기록됨
- Reset 버튼 작동
- 신기록 시 시각적 피드백

---

### Phase 7: 폴리싱 및 최적화 (Day 8-9)

#### Day 8: UI 개선
- [ ] 레드존 효과 (25~30km/h)
  - 바늘 빨간색으로 변경
  - 게이지 테두리 빨간색
- [ ] 부드러운 페이드 효과
- [ ] 터치 버튼 크기 조정 (최소 44×44px)
- [ ] 폰트 크기 미세 조정 (가독성)

#### Day 9: 성능 최적화
- [ ] OpenGL 렌더링 활성화
  ```cpp
  QSurfaceFormat format;
  format.setSamples(4);  // 안티앨리어싱
  QSurfaceFormat::setDefaultFormat(format);
  ```
- [ ] 불필요한 repaint 제거
- [ ] CPU 사용률 모니터링 (< 20%)
- [ ] 메모리 누수 확인 (valgrind)

**완료 기준:**
- 부드러운 60 FPS 유지
- CPU 사용률 안정적
- 시각적 완성도 높음

---

## 4. 데이터 흐름도

```
┌─────────────┐
│  Arduino    │
│  (LM393)    │
└──────┬──────┘
       │ Serial: "Speed: 84.00 pulse/s"
       ↓
┌──────────────────┐
│  SerialReader    │
│  (QSerialPort)   │
└──────┬───────────┘
       │ Signal: speedDataReceived(float)
       ↓
┌──────────────────┐      ┌─────────────────┐
│ DataProcessor    │←─────│ Calibration     │
│  - pulseToKmh()  │      │ Manager         │
│  - pulseToRPM()  │      │ (.json)         │
└──────┬───────────┘      └─────────────────┘
       │
       ├─→ SpeedometerWidget (바늘 + 숫자)
       ├─→ RpmGauge (RPM 게이지)
       └─→ MainWindow (최고 속도 등)


┌──────────────┐
│  Python      │
│  piracer.py  │
└──────┬───────┘
       │ stdout: {"voltage": 7.8, "percent": 85}
       ↓
┌──────────────────┐
│  QProcess        │
│  (JSON parser)   │
└──────┬───────────┘
       │
       └─→ BatteryWidget (배터리 표시)
```

---

## 5. 캘리브레이션 프로세스

### 5.1 캘리브레이션 파일 형식

**config/calibration.json:**
```json
{
  "version": "1.0",
  "speed": {
    "pulses_per_second_to_kmh": 0.72,
    "comment": "실측 후 수정 필요"
  },
  "rpm": {
    "pulses_per_revolution": 20,
    "comment": "바퀴 1회전당 센서 펄스 수"
  },
  "wheel": {
    "diameter_mm": 65,
    "circumference_mm": 204.2
  },
  "battery": {
    "v_min": 6.4,
    "v_max": 8.4,
    "cells": 2,
    "type": "LiPo 2S"
  }
}
```

### 5.2 캘리브레이션 절차

#### 속도 캘리브레이션
1. 정확한 직선 거리 측정 (10m 권장)
2. PiRacer를 일정 속도로 주행
3. 시간과 펄스 수 기록
4. 계산:
   ```
   실제속도 = 거리 / 시간  (m/s)
   실제속도_kmh = 실제속도 * 3.6
   FACTOR = 실제속도_kmh / pulsePerSec
   ```
5. `calibration.json` 업데이트

#### RPM 캘리브레이션
1. PiRacer 정지
2. 바퀴를 손으로 천천히 1회전
3. 시리얼 모니터에서 펄스 수 확인
4. 3회 반복 후 평균값
5. `calibration.json` 업데이트

---

## 6. 빌드 및 배포

### 6.1 빌드 환경 설정

#### Raspberry Pi OS에 Qt 설치
```bash
sudo apt update
sudo apt install -y qt5-default qtcreator
sudo apt install -y libqt5serialport5-dev
sudo apt install -y cmake build-essential git
```

#### Python 환경 설정
```bash
sudo pip3 install piracer-py pyserial
```

### 6.2 빌드 명령

#### CMake 빌드
```bash
cd Dashboard
mkdir build && cd build
cmake ..
make -j4
```

#### qmake 빌드
```bash
cd Dashboard
qmake dashboard.pro
make -j4
```

### 6.3 실행

```bash
# 권한 설정 (시리얼 포트)
sudo usermod -a -G dialout $USER
# 재로그인 필요

# 실행
./build/dashboard
```

### 6.4 자동 시작 설정 (선택)

**~/.config/autostart/dashboard.desktop:**
```ini
[Desktop Entry]
Type=Application
Name=PiRacer Dashboard
Exec=/home/pi/Dashboard/build/dashboard
Hidden=false
NoDisplay=false
X-GNOME-Autostart-enabled=true
```

---

## 7. 테스트 계획 (간략)

### 7.1 단위 테스트
- [ ] DataProcessor 함수 (pulseToKmh, pulseToRPM)
- [ ] SerialReader 파싱 로직
- [ ] CalibrationManager JSON 로드

### 7.2 통합 테스트
- [ ] Arduino + Qt 시리얼 통신
- [ ] Python + Qt 프로세스 통신
- [ ] 모든 위젯 동기화

### 7.3 시스템 테스트
- [ ] 실제 주행 테스트
- [ ] 2시간 연속 작동
- [ ] 케이블 연결/해제

**자세한 테스트는 `VERIFICATION_PLAN.md` 참조**

---

## 8. 리스크 및 대응 방안

### 8.1 기술적 리스크

| 리스크 | 확률 | 영향 | 대응 방안 |
|--------|------|------|-----------|
| Python 프로세스 통신 실패 | 중 | 높음 | 대안: ZMQ 또는 공유 메모리 사용 |
| 터치스크린 드라이버 문제 | 중 | 중 | 대안: 마우스 입력으로 테스트 |
| 성능 부족 (FPS 저하) | 낮 | 중 | OpenGL 활성화, 위젯 최적화 |
| 시리얼 포트 자동 감지 실패 | 중 | 낮 | 설정 파일로 포트 지정 |

### 8.2 하드웨어 리스크

| 리스크 | 대응 |
|--------|------|
| 센서 오동작 | 노이즈 필터링, 이상값 제거 |
| 배터리 전압 불안정 | 평균값 사용, 5초 간격 업데이트 |
| 터치스크린 해상도 불일치 | 스케일링 적용 |

---

## 9. 마일스톤

| 날짜 | 마일스톤 | 완료 기준 |
|------|----------|-----------|
| Day 1-2 | Phase 1 완료 | 정적 UI 표시 |
| Day 3 | Phase 2 완료 | 시리얼 데이터 수신 |
| Day 4 | Phase 3 완료 | 애니메이션 작동 |
| Day 5 | Phase 4 완료 | RPM + 보조 위젯 |
| Day 6 | Phase 5 완료 | 배터리 표시 |
| Day 7 | Phase 6 완료 | 통계 기능 |
| Day 8-9 | Phase 7 완료 | 완성 및 테스트 |

---

## 10. 체크리스트

### Phase 1: 기본 UI
- [ ] Qt 프로젝트 생성
- [ ] MainWindow 1200×400
- [ ] SpeedometerWidget 정적 그리기
- [ ] 폰트 리소스 추가
- [ ] 레이아웃 3칸 분할

### Phase 2: 시리얼 통신
- [ ] SerialReader 클래스
- [ ] 포트 자동 감지
- [ ] 데이터 파싱 정규표현식
- [ ] 시그널/슬롯 연결

### Phase 3: 애니메이션
- [ ] QPropertyAnimation 바늘
- [ ] 부드러운 회전 (200ms)
- [ ] 숫자 업데이트 (100ms)
- [ ] 60 FPS 확인

### Phase 4: RPM 및 보조
- [ ] DataProcessor 클래스
- [ ] RpmGauge 위젯
- [ ] 캘리브레이션 로드
- [ ] 주행 시간 타이머

### Phase 5: 배터리
- [ ] Python 브릿지 스크립트
- [ ] QProcess 연동
- [ ] BatteryWidget 그리기
- [ ] 색상 코드 적용

### Phase 6: 통계
- [ ] 최고 속도 추적
- [ ] Reset 버튼
- [ ] 신기록 애니메이션
- [ ] 주행 거리 계산

### Phase 7: 폴리싱
- [ ] 레드존 효과
- [ ] OpenGL 렌더링
- [ ] 성능 최적화
- [ ] 최종 테스트

---

## 11. 다음 단계 (Phase 2, 향후)

### 추가 기능
- [ ] 데이터 로깅 (CSV)
- [ ] 통계 화면 (평균 속도 등)
- [ ] 설정 화면 (캘리브레이션 조정)
- [ ] 방향 실시간 감지 (F/R 자동)
- [ ] 그래프 (속도 변화 추이)

### 개선 사항
- [ ] 단위 변환 (km/h ↔ mph)
- [ ] 색상 테마 변경
- [ ] 다국어 지원
- [ ] 원격 모니터링 (WiFi)

---

**작성자**: Ahn Hyunjun  
**검토**: ________________  
**승인**: ________________  
**날짜**: 2026-02-16
