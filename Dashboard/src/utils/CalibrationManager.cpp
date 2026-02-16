/**
 * @file CalibrationManager.cpp
 * @brief Calibration Manager Implementation
 * @author Ahn Hyunjun
 * @date 2026-02-16
 */

#include "CalibrationManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

CalibrationManager::CalibrationManager()
    : m_speedCalibration(0.72f)
    , m_pulsesPerRevolution(20)
    , m_batteryVMin(6.4f)
    , m_batteryVMax(8.4f)
{
}

bool CalibrationManager::load(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open calibration file:" << filename;
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "Invalid JSON in calibration file";
        return false;
    }
    
    QJsonObject root = doc.object();
    
    // Load speed calibration
    if (root.contains("speed")) {
        QJsonObject speed = root["speed"].toObject();
        m_speedCalibration = speed["pulses_per_second_to_kmh"].toDouble(0.72);
    }
    
    // Load RPM calibration
    if (root.contains("rpm")) {
        QJsonObject rpm = root["rpm"].toObject();
        m_pulsesPerRevolution = rpm["pulses_per_revolution"].toInt(20);
    }
    
    // Load battery calibration
    if (root.contains("battery")) {
        QJsonObject battery = root["battery"].toObject();
        m_batteryVMin = battery["v_min"].toDouble(6.4);
        m_batteryVMax = battery["v_max"].toDouble(8.4);
    }
    
    qDebug() << "Calibration loaded successfully from" << filename;
    return true;
}

bool CalibrationManager::save(const QString &filename) const
{
    QJsonObject root;
    
    // Speed calibration
    QJsonObject speed;
    speed["pulses_per_second_to_kmh"] = m_speedCalibration;
    speed["comment"] = "Measured value";
    root["speed"] = speed;
    
    // RPM calibration
    QJsonObject rpm;
    rpm["pulses_per_revolution"] = m_pulsesPerRevolution;
    rpm["comment"] = "Pulses per 1 wheel rotation";
    root["rpm"] = rpm;
    
    // Battery calibration
    QJsonObject battery;
    battery["v_min"] = m_batteryVMin;
    battery["v_max"] = m_batteryVMax;
    battery["cells"] = 2;
    battery["type"] = "LiPo 2S";
    root["battery"] = battery;
    
    root["version"] = "1.0";
    
    QJsonDocument doc(root);
    
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to write calibration file:" << filename;
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    qDebug() << "Calibration saved to" << filename;
    return true;
}
