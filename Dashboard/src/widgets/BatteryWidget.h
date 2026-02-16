/**
 * @file BatteryWidget.h
 * @brief Battery Status Display Widget
 * @author Ahn Hyunjun
 * @date 2026-02-16
 */

#ifndef BATTERYWIDGET_H
#define BATTERYWIDGET_H

#include <QWidget>
#include <QTimer>

/**
 * @class BatteryWidget
 * @brief Battery level and voltage display
 * 
 * Features:
 * - Battery icon
 * - Percentage display
 * - Voltage display
 * - Color-coded (green/yellow/orange/red)
 * - Warning animation for low battery
 */
class BatteryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BatteryWidget(QWidget *parent = nullptr);
    
    void setBattery(float percent, float voltage);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    
private slots:
    void onBlinkTimer();
    
private:
    void drawBatteryIcon(QPainter *painter);
    void drawPercentage(QPainter *painter);
    void drawVoltage(QPainter *painter);
    
    QColor getBatteryColor() const;
    
    float m_percent;
    float m_voltage;
    bool m_blinkState;
    QTimer *m_blinkTimer;
};

#endif // BATTERYWIDGET_H
