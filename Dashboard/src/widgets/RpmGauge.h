/**
 * @file RpmGauge.h
 * @brief RPM Gauge Widget (Semi-circle)
 * @author Ahn Hyunjun
 * @date 2026-02-16
 */

#ifndef RPMGAUGE_H
#define RPMGAUGE_H

#include <QWidget>

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

public:
    explicit RpmGauge(QWidget *parent = nullptr);
    
    void setRPM(float rpm);
    float rpm() const { return m_rpm; }
    
protected:
    void paintEvent(QPaintEvent *event) override;
    
private:
    void drawGauge(QPainter *painter);
    void drawValue(QPainter *painter);
    
    float m_rpm;
    
    static constexpr float MAX_RPM = 500.0f;
};

#endif // RPMGAUGE_H
