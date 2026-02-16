/**
 * @file BatteryWidget.cpp
 * @brief Battery Widget Implementation
 * @author Ahn Hyunjun
 * @date 2026-02-16
 */

#include "BatteryWidget.h"
#include <QPainter>
#include <QFont>

BatteryWidget::BatteryWidget(QWidget *parent)
    : QWidget(parent)
    , m_percent(100.0f)
    , m_voltage(8.4f)
    , m_blinkState(true)
    , m_blinkTimer(nullptr)
{
    // Setup blink timer for low battery warning
    m_blinkTimer = new QTimer(this);
    connect(m_blinkTimer, &QTimer::timeout, this, &BatteryWidget::onBlinkTimer);
}

void BatteryWidget::setBattery(float percent, float voltage)
{
    m_percent = qBound(0.0f, percent, 100.0f);
    m_voltage = voltage;
    
    // Start/stop blink animation for low battery
    if (m_percent < 20.0f && !m_blinkTimer->isActive()) {
        m_blinkTimer->start(1000);  // 1 second interval
    } else if (m_percent >= 20.0f && m_blinkTimer->isActive()) {
        m_blinkTimer->stop();
        m_blinkState = true;
    }
    
    update();
}

void BatteryWidget::onBlinkTimer()
{
    m_blinkState = !m_blinkState;
    update();
}

void BatteryWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    drawBatteryIcon(&painter);
    drawPercentage(&painter);
    drawVoltage(&painter);
}

void BatteryWidget::drawBatteryIcon(QPainter *painter)
{
    const int cx = width() / 2;
    const int cy = 18;
    const int w = 64;
    const int h = 24;
    
    painter->save();
    
    // Get battery color
    QColor color = getBatteryColor();
    
    // Apply blink effect for low battery
    if (m_percent < 20.0f && !m_blinkState) {
        painter->setOpacity(0.3);
    }
    
    // Draw battery body
    QPen pen(color, 2);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(cx - w/2, cy - h/2, w, h);
    
    // Draw battery terminal
    painter->drawRect(cx + w/2, cy - h/4, 6, h/2);
    
    // Draw fill level
    int fillWidth = static_cast<int>((w - 8) * (m_percent / 100.0f));
    if (fillWidth > 0) {
        painter->setBrush(color);
        painter->setPen(Qt::NoPen);
        painter->drawRect(cx - w/2 + 4, cy - h/2 + 4, fillWidth, h - 8);
    }
    
    painter->restore();
}

void BatteryWidget::drawPercentage(QPainter *painter)
{
    const int cx = width() / 2;
    const int cy = 48;
    
    painter->save();
    
    // Get color
    QColor color = getBatteryColor();
    
    // Apply blink effect
    if (m_percent < 20.0f && !m_blinkState) {
        painter->setOpacity(0.3);
    }
    
    // Draw percentage
    QFont font("Roboto", 18, QFont::Bold);
    painter->setFont(font);
    painter->setPen(color);
    
    QString percentText = QString::number(m_percent, 'f', 1) + "%";
    QRect rect(cx - 80, cy - 14, 160, 28);
    painter->drawText(rect, Qt::AlignCenter, percentText);
    
    painter->restore();
}

void BatteryWidget::drawVoltage(QPainter *painter)
{
    const int cx = width() / 2;
    const int cy = 72;
    
    painter->save();
    
    // Draw voltage
    QFont font("Roboto", 11);
    painter->setFont(font);
    painter->setPen(QColor("#7A8A9E"));
    
    QString voltageText = QString::number(m_voltage, 'f', 1) + "V";
    QRect rect(cx - 60, cy - 10, 120, 20);
    painter->drawText(rect, Qt::AlignCenter, voltageText);
    
    painter->restore();
}

QColor BatteryWidget::getBatteryColor() const
{
    if (m_percent >= 80.0f) {
        return QColor("#00FF88");  // Green
    } else if (m_percent >= 50.0f) {
        return QColor("#FFD700");  // Yellow
    } else if (m_percent >= 20.0f) {
        return QColor("#FF8800");  // Orange
    } else {
        return QColor("#FF3B3B");  // Red
    }
}
