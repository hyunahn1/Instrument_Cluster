/**
 * @file DataProcessor.h
 * @brief Data Conversion and Processing
 * @author Ahn Hyunjun
 * @date 2026-02-16
 */

#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <QObject>

/**
 * @class DataProcessor
 * @brief Converts raw sensor data to display units
 * 
 * Features:
 * - Pulse/s to km/h conversion
 * - Pulse/s to RPM conversion
 * - Distance calculation
 */
class DataProcessor : public QObject
{
    Q_OBJECT

public:
    explicit DataProcessor(QObject *parent = nullptr);
    
    // Conversion functions
    float pulseToKmh(float pulsePerSec) const;
    float pulseToRPM(float pulsePerSec) const;
    
    // Calibration
    void setSpeedCalibration(float factor);
    void setPulsesPerRevolution(int pulses);
    
    float speedCalibration() const { return m_speedCalibrationFactor; }
    int pulsesPerRevolution() const { return m_pulsesPerRevolution; }
    
private:
    float m_speedCalibrationFactor;
    int m_pulsesPerRevolution;
};

#endif // DATAPROCESSOR_H
