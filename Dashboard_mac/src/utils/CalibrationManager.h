/**
 * @file CalibrationManager.h
 * @brief Calibration Data Manager
 * @author Ahn Hyunjun
 * @date 2026-02-16
 */

#ifndef CALIBRATIONMANAGER_H
#define CALIBRATIONMANAGER_H

#include <QString>

/**
 * @class CalibrationManager
 * @brief Loads and saves calibration data from JSON file
 */
class CalibrationManager
{
public:
    CalibrationManager();
    
    bool load(const QString &filename);
    bool save(const QString &filename) const;
    
    // Getters
    float speedCalibration() const { return m_speedCalibration; }
    int pulsesPerRevolution() const { return m_pulsesPerRevolution; }
    float batteryVMin() const { return m_batteryVMin; }
    float batteryVMax() const { return m_batteryVMax; }
    
    // Setters
    void setSpeedCalibration(float value) { m_speedCalibration = value; }
    void setPulsesPerRevolution(int value) { m_pulsesPerRevolution = value; }
    void setBatteryVMin(float value) { m_batteryVMin = value; }
    void setBatteryVMax(float value) { m_batteryVMax = value; }
    
private:
    float m_speedCalibration;
    int m_pulsesPerRevolution;
    float m_batteryVMin;
    float m_batteryVMax;
};

#endif // CALIBRATIONMANAGER_H
