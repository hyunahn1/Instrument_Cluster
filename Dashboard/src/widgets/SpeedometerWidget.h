/**
 * @file SpeedometerWidget.h
 * @brief Central Speedometer Widget with Analog Needle and Digital Display
 * @author Ahn Hyunjun
 * @date 2026-02-16
 */

#ifndef SPEEDOMETERWIDGET_H
#define SPEEDOMETERWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>

/**
 * @class SpeedometerWidget
 * @brief Hybrid speedometer with analog gauge and digital number
 * 
 * Features:
 * - Circular gauge (270° arc)
 * - Animated needle
 * - Digital speed display in center
 * - Red zone for high speeds (25-30 km/h)
 */
class SpeedometerWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float needleAngle READ needleAngle WRITE setNeedleAngle)

public:
    explicit SpeedometerWidget(QWidget *parent = nullptr);
    
    void setSpeed(float speedKmh);
    float speed() const { return m_speed; }
    
    float needleAngle() const { return m_needleAngle; }
    void setNeedleAngle(float angle);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    
private:
    void drawGauge(QPainter *painter);
    void drawTicks(QPainter *painter);
    void drawNeedle(QPainter *painter);
    void drawShiftLights(QPainter *painter);
    void drawDigitalSpeed(QPainter *painter);
    
    float m_speed;
    float m_needleAngle;
    float m_lastTargetAngle;
    bool m_startupAnimationDone;
    QPropertyAnimation *m_needleAnimation;
    
    // Constants
    static constexpr float MAX_SPEED = 30.0f;      // km/h
    static constexpr float RED_ZONE_START = 25.0f; // km/h
    static constexpr float GAUGE_START_ANGLE = 135.0f;  // degrees
    static constexpr float GAUGE_SPAN_ANGLE = 270.0f;   // degrees
};

#endif // SPEEDOMETERWIDGET_H
