/**
 * @file RpmGauge.h
 * @brief RPM Gauge Widget (Semi-circle)
 * @author Ahn Hyunjun
 * @date 2026-02-16
 */

#ifndef RPMGAUGE_H
#define RPMGAUGE_H

#include <QWidget>
#include <QPropertyAnimation>

/**
 * @class RpmGauge
 * @brief Semi-circle RPM gauge for wheel rotation speed
 * 
 * Features:
 * - 180° semi-circle gauge
 * - Range: 0-500 RPM
 * - Cyan blue color theme
 */
class RpmGauge : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float needleAngle READ needleAngle WRITE setNeedleAngle)

public:
    explicit RpmGauge(QWidget *parent = nullptr);
    
    void setRPM(float rpm);
    float rpm() const { return m_rpm; }
    float needleAngle() const { return m_needleAngle; }
    void setNeedleAngle(float angle);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    
private:
    void drawGauge(QPainter *painter);
    void drawValue(QPainter *painter);
    
    float m_rpm;
    float m_needleAngle;
    QPropertyAnimation *m_needleAnimation;
    
    // Dashboard display range tuned for current wheel RPM signal.
    static constexpr float MAX_RPM = 120.0f;
};

#endif // RPMGAUGE_H
