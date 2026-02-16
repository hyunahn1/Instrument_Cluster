/**
 * @file DataProcessor.cpp
 * @brief Data Processor Implementation
 * @author Ahn Hyunjun
 * @date 2026-02-16
 */

#include "DataProcessor.h"
#include "CalibrationManager.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

DataProcessor::DataProcessor(QObject *parent)
    : QObject(parent)
    , m_speedCalibrationFactor(0.72f)  // Default value (needs calibration)
    , m_pulsesPerRevolution(20)        // Default value (needs calibration)
{
    // Try to load calibration from common runtime locations
    CalibrationManager calibration;
    const QString appDir = QCoreApplication::applicationDirPath();
    const QStringList candidateConfigs = {
        "config/calibration.json",
        QDir::cleanPath(appDir + "/config/calibration.json"),
        QDir::cleanPath(appDir + "/../config/calibration.json"),
        QDir::cleanPath(appDir + "/../../config/calibration.json")
    };

    QString calibrationPath;
    for (const QString &candidate : candidateConfigs) {
        if (QFileInfo::exists(candidate)) {
            calibrationPath = candidate;
            break;
        }
    }

    if (!calibrationPath.isEmpty() && calibration.load(calibrationPath)) {
        m_speedCalibrationFactor = calibration.speedCalibration();
        m_pulsesPerRevolution = calibration.pulsesPerRevolution();
        
        qDebug() << "Loaded calibration:";
        qDebug() << "  File:" << calibrationPath;
        qDebug() << "  Speed factor:" << m_speedCalibrationFactor;
        qDebug() << "  Pulses/rev:" << m_pulsesPerRevolution;
    } else {
        qWarning() << "Using default calibration values";
    }
}

float DataProcessor::pulseToKmh(float pulsePerSec) const
{
    // Convert pulse/s to km/h using calibration factor
    return pulsePerSec * m_speedCalibrationFactor;
}

float DataProcessor::pulseToRPM(float pulsePerSec) const
{
    // Convert pulse/s to RPM
    // RPM = (pulse/s × 60) / pulses_per_revolution
    if (m_pulsesPerRevolution == 0) {
        return 0.0f;
    }
    
    return (pulsePerSec * 60.0f) / m_pulsesPerRevolution;
}

void DataProcessor::setSpeedCalibration(float factor)
{
    m_speedCalibrationFactor = factor;
}

void DataProcessor::setPulsesPerRevolution(int pulses)
{
    m_pulsesPerRevolution = pulses;
}
