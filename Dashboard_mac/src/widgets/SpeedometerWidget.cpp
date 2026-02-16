/**
 * @file SpeedometerWidget.cpp
 * @brief Speedometer Widget Implementation
 * @author Ahn Hyunjun
 * @date 2026-02-16
 */

#include "SpeedometerWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <QFontDatabase>
#include <QtMath>

SpeedometerWidget::SpeedometerWidget(QWidget *parent)
    : QWidget(parent)
    , m_speed(0.0f)
    , m_needleAngle(0.0f)
    , m_needleAnimation(nullptr)
{
    // Setup needle animation
    m_needleAnimation = new QPropertyAnimation(this, "needleAngle");
    m_needleAnimation->setDuration(200);
    m_needleAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void SpeedometerWidget::setSpeed(float speedKmh)
{
    // Clamp speed to valid range
    speedKmh = qBound(0.0f, speedKmh, MAX_SPEED);
    m_speed = speedKmh;
    
    // Calculate target needle angle (0-270°)
    float targetAngle = (speedKmh / MAX_SPEED) * GAUGE_SPAN_ANGLE;
    
    // Animate needle
    m_needleAnimation->stop();
    m_needleAnimation->setStartValue(m_needleAngle);
    m_needleAnimation->setEndValue(targetAngle);
    m_needleAnimation->start();
}

void SpeedometerWidget::setNeedleAngle(float angle)
{
    m_needleAngle = angle;
    update();  // Trigger repaint
}

void SpeedometerWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw components
    drawGauge(&painter);
    drawTicks(&painter);
    drawNeedle(&painter);
    drawDigitalSpeed(&painter);
}

void SpeedometerWidget::drawGauge(QPainter *painter)
{
    int cx = width() / 2;
    int cy = height() / 2;
    int radius = qMin(width(), height()) / 2 - 20;
    
    painter->save();
    painter->translate(cx, cy);
    
    // Draw outer circle (silver)
    QPen pen(QColor("#C0C8D0"), 3);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(QPointF(0, 0), radius, radius);
    
    // Draw arc background
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor("#1A1E2A"));
    
    QPainterPath arcPath;
    arcPath.arcMoveTo(-radius, -radius, radius * 2, radius * 2, GAUGE_START_ANGLE);
    arcPath.arcTo(-radius, -radius, radius * 2, radius * 2, GAUGE_START_ANGLE, GAUGE_SPAN_ANGLE);
    painter->drawPath(arcPath);
    
    // Draw red zone arc
    float redZoneStartAngle = GAUGE_START_ANGLE + (RED_ZONE_START / MAX_SPEED) * GAUGE_SPAN_ANGLE;
    float redZoneSpan = ((MAX_SPEED - RED_ZONE_START) / MAX_SPEED) * GAUGE_SPAN_ANGLE;
    
    pen.setColor(QColor("#FF3B3B"));
    pen.setWidth(8);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    
    painter->drawArc(-radius + 10, -radius + 10, (radius - 10) * 2, (radius - 10) * 2,
                    static_cast<int>(redZoneStartAngle * 16),
                    static_cast<int>(redZoneSpan * 16));
    
    painter->restore();
}

void SpeedometerWidget::drawTicks(QPainter *painter)
{
    int cx = width() / 2;
    int cy = height() / 2;
    int radius = qMin(width(), height()) / 2 - 20;
    
    painter->save();
    painter->translate(cx, cy);
    
    // Draw major ticks (0, 5, 10, 15, 20, 25, 30)
    for (int speed = 0; speed <= static_cast<int>(MAX_SPEED); speed += 5) {
        painter->save();
        
        float angle = GAUGE_START_ANGLE + (speed / MAX_SPEED) * GAUGE_SPAN_ANGLE;
        painter->rotate(angle);
        
        // Major tick
        QPen pen(QColor("#E8F0FF"), 2);
        painter->setPen(pen);
        painter->drawLine(0, -radius + 15, 0, -radius + 35);
        
        // Speed label
        painter->rotate(-angle);  // Reset rotation for text
        float textAngle = angle * M_PI / 180.0f;
        int textX = static_cast<int>((radius - 55) * qCos(textAngle));
        int textY = static_cast<int>((radius - 55) * qSin(textAngle));
        
        QFont font("Roboto", 12);
        painter->setFont(font);
        painter->setPen(QColor("#E8F0FF"));
        
        QString label = QString::number(speed);
        QRect textRect(textX - 20, textY - 10, 40, 20);
        painter->drawText(textRect, Qt::AlignCenter, label);
        
        painter->restore();
    }
    
    // Draw minor ticks (every 1 km/h)
    for (int speed = 0; speed <= static_cast<int>(MAX_SPEED); ++speed) {
        if (speed % 5 == 0) continue;  // Skip major ticks
        
        painter->save();
        
        float angle = GAUGE_START_ANGLE + (speed / MAX_SPEED) * GAUGE_SPAN_ANGLE;
        painter->rotate(angle);
        
        QPen pen(QColor("#7A8A9E"), 1);
        painter->setPen(pen);
        painter->drawLine(0, -radius + 20, 0, -radius + 30);
        
        painter->restore();
    }
    
    painter->restore();
}

void SpeedometerWidget::drawNeedle(QPainter *painter)
{
    int cx = width() / 2;
    int cy = height() / 2;
    int radius = qMin(width(), height()) / 2 - 20;
    
    painter->save();
    painter->translate(cx, cy);
    
    // Align needle with gauge tick angle space (Qt rotation starts from upward vector).
    float needleAngle = GAUGE_START_ANGLE + m_needleAngle + 90.0f;
    painter->rotate(needleAngle);
    
    // Needle color (red if in red zone)
    QColor needleColor = (m_speed >= RED_ZONE_START) ? QColor("#FF3B3B") : QColor("#FFFFFF");
    
    // Draw needle
    QPainterPath needlePath;
    needlePath.moveTo(0, 0);
    needlePath.lineTo(-3, -10);
    needlePath.lineTo(0, -radius + 40);
    needlePath.lineTo(3, -10);
    needlePath.closeSubpath();
    
    painter->setPen(Qt::NoPen);
    painter->setBrush(needleColor);
    painter->drawPath(needlePath);
    
    // Center circle
    painter->setBrush(QColor("#C0C8D0"));
    painter->drawEllipse(QPointF(0, 0), 8, 8);
    
    painter->restore();
}

void SpeedometerWidget::drawDigitalSpeed(QPainter *painter)
{
    int cx = width() / 2;
    int cy = height() / 2 + 80;
    
    painter->save();
    
    // Draw speed number
    QFont font("Roboto", 72, QFont::Bold);
    painter->setFont(font);
    painter->setPen(QColor("#E8F0FF"));
    
    QString speedText = QString::number(static_cast<int>(m_speed));
    QRect speedRect(cx - 150, cy - 50, 300, 80);
    painter->drawText(speedRect, Qt::AlignCenter, speedText);
    
    // Draw unit
    font.setPointSize(24);
    font.setWeight(QFont::Normal);
    painter->setFont(font);
    
    QRect unitRect(cx - 100, cy + 30, 200, 40);
    painter->drawText(unitRect, Qt::AlignCenter, "km/h");
    
    painter->restore();
}
