/**
 * @file main.cpp
 * @brief PiRacer Speed Sensor System using LM393 IR Sensor
 * @author Ahn Hyunjun
 * @date 2026-02-12
 * @version 1.0.0
 * 
 * @description
 * Real-time speed measurement system for PiRacer autonomous vehicle.
 * Uses LM393 infrared speed sensor with interrupt-based pulse counting
 * for accurate speed monitoring via serial communication.
 * 
 * @hardware
 * - Arduino Uno/Nano (ATmega328P)
 * - LM393 Speed Sensor Module
 * - PiRacer Platform
 * 
 * @pin_configuration
 * LM393 DO -> Arduino Pin 2 (INT0)
 * LM393 VCC -> 5V
 * LM393 GND -> GND
 */

#include <Arduino.h>

// ==================== Configuration ====================

// Pin Definitions
#define SPEED_SENSOR_PIN    2       // Digital pin 2 (interrupt capable)
#define SERIAL_BAUD_RATE    9600    // Serial communication baud rate

// Timing Configuration
#define UPDATE_INTERVAL_MS  500     // Speed update interval (milliseconds)

// ==================== Global Variables ====================

// Speed measurement variables
volatile unsigned long g_pulseCount = 0;    // Pulse counter (modified by ISR)
unsigned long g_lastUpdateTime = 0;         // Last speed calculation timestamp
float g_currentSpeed = 0.0;                 // Current speed (pulses per second)

// ==================== Interrupt Service Routine ====================

/**
 * @brief Interrupt Service Routine for pulse counting
 * @note Called automatically on FALLING edge detection
 * @warning Keep ISR short and avoid Serial/delay operations
 */
void ISR_countPulse() {
  g_pulseCount++;
}

// ==================== Setup Function ====================

/**
 * @brief Initialize hardware and serial communication
 */
void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial) {
    ; // Wait for serial port to connect (needed for native USB)
  }
  
  // Configure sensor pin
  pinMode(SPEED_SENSOR_PIN, INPUT);
  
  // Attach interrupt on falling edge (HIGH to LOW transition)
  attachInterrupt(digitalPinToInterrupt(SPEED_SENSOR_PIN), 
                  ISR_countPulse, 
                  FALLING);
  
  // Print startup banner
  printStartupBanner();
  
  // Initialize timestamp
  g_lastUpdateTime = millis();
}

// ==================== Main Loop ====================

/**
 * @brief Main program loop - calculates and displays speed
 */
void loop() {
  unsigned long currentTime = millis();
  
  // Check if update interval has elapsed
  if (currentTime - g_lastUpdateTime >= UPDATE_INTERVAL_MS) {
    calculateSpeed();
    displaySpeed();
    resetCounters();
    
    g_lastUpdateTime = currentTime;
  }
}

// ==================== Helper Functions ====================

/**
 * @brief Calculate speed from pulse count
 */
void calculateSpeed() {
  // Convert to pulses per second
  // Formula: (pulses / interval_seconds) = pulses per second
  float intervalSeconds = UPDATE_INTERVAL_MS / 1000.0;
  g_currentSpeed = g_pulseCount / intervalSeconds;
}

/**
 * @brief Display speed data on serial monitor
 */
void displaySpeed() {
  Serial.print("Pulses: ");
  Serial.print(g_pulseCount);
  Serial.print(" | Speed: ");
  Serial.print(g_currentSpeed, 2);  // 2 decimal places
  Serial.print(" pulse/s | Time: ");
  Serial.print(millis() / 1000.0, 2);
  Serial.println(" s");
}

/**
 * @brief Reset pulse counter for next measurement cycle
 */
void resetCounters() {
  g_pulseCount = 0;
}

/**
 * @brief Print startup banner to serial monitor
 */
void printStartupBanner() {
  Serial.println(F("========================================="));
  Serial.println(F("   PiRacer Speed Sensor System v1.0     "));
  Serial.println(F("========================================="));
  Serial.println(F("Sensor: LM393 IR Speed Sensor"));
  Serial.println(F("Platform: Arduino Uno (ATmega328P)"));
  Serial.print(F("Update Interval: "));
  Serial.print(UPDATE_INTERVAL_MS);
  Serial.println(F(" ms"));
  Serial.println(F("========================================="));
  Serial.println(F("Starting measurements..."));
  Serial.println();
}