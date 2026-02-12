# Wokwi 웹에서 테스트하기 (무료)

## 1단계: Wokwi 웹사이트 접속
https://wokwi.com/new/arduino-uno

## 2단계: 코드 복사 붙여넣기

왼쪽 에디터에 아래 코드를 붙여넣으세요:

```cpp
const int LED_PIN = 13;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("LED 깜빡이기 시작!");
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED ON");
  delay(1000);
  
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED OFF");
  delay(1000);
}
```

## 3단계: 시뮬레이션 실행
- 초록색 "Start Simulation" 버튼 클릭!
- LED가 깜빡이는 걸 볼 수 있어요

## 시리얼 모니터 보기
- 하단의 "Serial Monitor" 탭 클릭
