/**
 * @file RpmGauge.cpp
 * @brief RPM Gauge Implementation
 * @author Ahn Hyunjun
 * @date 2026-02-16
 */

#include "RpmGauge.h"
#include <QPainter>
#include <QFont>
#include <QtMath>

RpmGauge::RpmGauge(QWidget *parent)
    : QWidget(parent)
    , m_rpm(0.0f)
{
}

void RpmGauge::setRPM(float rpm)
{
    m_rpm = qBound(0.0f, rpm, MAX_RPM);
    update();
}

void RpmGauge::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    drawGauge(&painter);
    drawValue(&painter);
}

void RpmGauge::drawGauge(QPainter *painter)
{
    int cx = width() / 2;
    int cy = height() - 20;
    int radius = qMin(width(), height()) - 40;
    
    painter->save();
    painter->translate(cx, cy);
    
    // Draw arc (180°)
    QPen pen(QColor("#00D4FF"), 4);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    
    // Background arc
    pen.setColor(QColor("#2A2E3A"));
    painter->setPen(pen);
    painter->drawArc(-radius, -radius, radius * 2, radius * 2, 0, 180 * 16);
    
    // Active arc (based on RPM)
    float arcSpan = (m_rpm / MAX_RPM) * 180.0f;
    pen.setColor(QColor("#00D4FF"));
    pen.setWidth(6);
    painter->setPen(pen);
    painter->drawArc(-radius, -radius, radius * 2, radius * 2, 0, static_cast<int>(arcSpan * 16));
    
    // Draw ticks
    for (int i = 0; i <= 5; ++i) {
        painter->save();
        float angle = i * 36.0f;  // 180° / 5 = 36°
        painter->rotate(-angle);
        
        pen.setColor(QColor("#7A8A9E"));
        pen.setWidth(2);
        painter->setPen(pen);
        painter->drawLine(0, -radius + 10, 0, -radius + 20);
        
        painter->restore();
    }
    
    painter->restore();
}

void RpmGauge::drawValue(QPainter *painter)
{
    int cx = width() / 2;
    int cy = height() - 30;
    
    painter->save();
    
    // Draw RPM number
    QFont font("Roboto", 48, QFont::Bold);
    painter->setFont(font);
    painter->setPen(QColor("#00D4FF"));
    
    QString rpmText = QString::number(static_cast<int>(m_rpm));
    QRect rpmRect(cx - 100, cy - 60, 200, 60);
    painter->drawText(rpmRect, Qt::AlignCenter, rpmText);
    
    // Draw label
    font.setPointSize(16);
    font.setWeight(QFont::Normal);
    painter->setFont(font);
    painter->setPen(QColor("#7A8A9E"));
    
    QRect labelRect(cx - 100, cy, 200, 30);
    painter->drawText(labelRect, Qt::AlignCenter, "Wheel RPM");
    
    painter->restore();
}
