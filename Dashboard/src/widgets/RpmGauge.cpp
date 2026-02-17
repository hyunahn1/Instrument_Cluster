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
#include <QEasingCurve>

RpmGauge::RpmGauge(QWidget *parent)
    : QWidget(parent)
    , m_rpm(0.0f)
    , m_needleAngle(200.0f)
    , m_needleAnimation(nullptr)
{
    m_needleAnimation = new QPropertyAnimation(this, "needleAngle");
    m_needleAnimation->setDuration(180);
    m_needleAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void RpmGauge::setRPM(float rpm)
{
    m_rpm = qBound(0.0f, rpm, MAX_RPM);
    // Motorsport-style sweep with slight headroom.
    const float startAngle = 200.0f;
    const float endAngle = -20.0f;
    const float displayMax = MAX_RPM * 1.15f;
    const float normalized = qBound(0.0f, m_rpm / displayMax, 1.0f);
    const float targetAngle = startAngle + (endAngle - startAngle) * normalized;
    m_needleAnimation->stop();
    m_needleAnimation->setStartValue(m_needleAngle);
    m_needleAnimation->setEndValue(targetAngle);
    m_needleAnimation->start();
}

void RpmGauge::setNeedleAngle(float angle)
{
    m_needleAngle = qBound(-20.0f, angle, 200.0f);
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
    const int cx = width() / 2;
    const int cy = static_cast<int>(height() * 0.78f);
    const int radius = qMin(static_cast<int>(width() * 0.48f), static_cast<int>(height() * 0.70f));
    const float startDeg = 200.0f;
    const float endDeg = -20.0f;
    
    painter->save();
    painter->translate(cx, cy);
    
    QPen pen;
    painter->setBrush(Qt::NoBrush);
    
    // Background arc
    pen.setColor(QColor("#2A2E3A"));
    pen.setWidth(4);
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);
    painter->drawArc(-radius, -radius, radius * 2, radius * 2,
                     static_cast<int>(startDeg * 16),
                     static_cast<int>((endDeg - startDeg) * 16));
    
    // Active arc
    float normalized = qBound(0.0f, m_rpm / (MAX_RPM * 1.15f), 1.0f);
    float angleNow = startDeg + (endDeg - startDeg) * normalized;
    pen.setColor(QColor("#00D4FF"));
    pen.setWidth(6);
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);
    painter->drawArc(-radius, -radius, radius * 2, radius * 2,
                     static_cast<int>(startDeg * 16),
                     static_cast<int>((angleNow - startDeg) * 16));
    
    // Tick marks
    constexpr int tickCount = 24;
    for (int i = 0; i <= tickCount; ++i) {
        const float t = static_cast<float>(i) / tickCount;
        const float angleDeg = startDeg + (endDeg - startDeg) * t;
        const float a = qDegreesToRadians(angleDeg);
        const bool major = (i % 4 == 0);
        const float r1 = radius - (major ? 19.0f : 15.0f);
        const float r2 = radius - 7.0f;
        const QPointF p1(qCos(a) * r1, -qSin(a) * r1);
        const QPointF p2(qCos(a) * r2, -qSin(a) * r2);

        pen.setColor(major ? QColor("#B5C5D8") : QColor("#5D6F86"));
        pen.setWidth(major ? 2 : 1);
        painter->setPen(pen);
        painter->drawLine(p1, p2);
    }

    // Needle tip segment only (keeps center area clean).
    QColor needleColor = (m_rpm > (MAX_RPM * 0.8f)) ? QColor("#FF4E5F") : QColor("#EAF6FF");
    painter->setPen(QPen(needleColor, 3, Qt::SolidLine, Qt::RoundCap));
    const float a = qDegreesToRadians(m_needleAngle);
    const float innerLen = radius - 55.0f;
    const float outerLen = radius - 28.0f;
    const QPointF p1(qCos(a) * innerLen, -qSin(a) * innerLen);
    const QPointF p2(qCos(a) * outerLen, -qSin(a) * outerLen);
    painter->drawLine(p1, p2);
    
    painter->restore();
}

void RpmGauge::drawValue(QPainter *painter)
{
    const int cx = width() / 2;
    const int cy = static_cast<int>(height() * 0.74f);
    
    painter->save();
    
    // Draw RPM number
    QFont font("Roboto", 46, QFont::Bold);
    painter->setFont(font);
    painter->setPen(QColor("#00D4FF"));
    
    QString rpmText = QString::number(static_cast<int>(m_rpm));
    QRect rpmRect(cx - 100, cy - 62, 200, 60);
    painter->drawText(rpmRect, Qt::AlignCenter, rpmText);
    
    // Place label directly under the RPM number.
    QRect labelRect(cx - 72, cy - 8, 144, 20);
    font.setPointSize(9);
    font.setWeight(QFont::Medium);
    painter->setFont(font);
    painter->setPen(QColor("#9FB4CB"));
    painter->drawText(labelRect, Qt::AlignHCenter | Qt::AlignVCenter, "Wheel RPM");
    
    painter->restore();
}
