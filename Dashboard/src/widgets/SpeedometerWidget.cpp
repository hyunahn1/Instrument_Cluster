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
#include <QtMath>

SpeedometerWidget::SpeedometerWidget(QWidget *parent)
    : QWidget(parent)
    , m_speed(0.0f)
    // Start from 6 o'clock-like position and animate to zero speed on first update.
    , m_needleAngle(-45.0f)
    , m_lastTargetAngle(-9999.0f)
    , m_startupAnimationDone(false)
    , m_needleAnimation(nullptr)
{
    // Setup needle animation
    m_needleAnimation = new QPropertyAnimation(this, "needleAngle");
    m_needleAnimation->setDuration(220);
    m_needleAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void SpeedometerWidget::setSpeed(float speedKmh)
{
    // Clamp speed to valid range
    speedKmh = qBound(0.0f, speedKmh, MAX_SPEED);
    m_speed = speedKmh;
    
    // Calculate target needle angle (0-270°)
    float targetAngle = (speedKmh / MAX_SPEED) * GAUGE_SPAN_ANGLE;

    // Avoid restarting the same animation target every frame.
    if (qAbs(targetAngle - m_lastTargetAngle) < 0.05f) {
        return;
    }
    m_lastTargetAngle = targetAngle;
    
    // Animate needle
    m_needleAnimation->stop();
    m_needleAnimation->setStartValue(m_needleAngle);
    m_needleAnimation->setEndValue(targetAngle);
    if (!m_startupAnimationDone) {
        // Startup sweep: slower, ignition-like movement.
        m_needleAnimation->setDuration(1150);
        m_needleAnimation->setEasingCurve(QEasingCurve::InOutCubic);
        m_startupAnimationDone = true;
    } else {
        m_needleAnimation->setDuration(220);
        m_needleAnimation->setEasingCurve(QEasingCurve::OutCubic);
    }
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
    drawShiftLights(&painter);
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

    // Inner dial fill with radial depth (AMG-inspired dark metallic feel)
    QRadialGradient dialGrad(QPointF(0, -radius * 0.15f), radius * 1.1f);
    dialGrad.setColorAt(0.0, QColor("#273247"));
    dialGrad.setColorAt(0.45, QColor("#141D2D"));
    dialGrad.setColorAt(1.0, QColor("#070C16"));
    painter->setPen(Qt::NoPen);
    painter->setBrush(dialGrad);
    painter->drawEllipse(QPointF(0, 0), radius - 12, radius - 12);
    
    // Draw outer ring (brushed-metal look)
    QPen pen(QColor("#A7B3C2"), 2);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(QPointF(0, 0), radius, radius);

    // Inner ring for depth
    pen.setColor(QColor(52, 67, 87, 180));
    pen.setWidth(2);
    painter->setPen(pen);
    painter->drawEllipse(QPointF(0, 0), radius - 10, radius - 10);
    
    // Subtle cyan outer accent arc for motorsport feel.
    QPen arcPen(QColor(0, 212, 255, 90), 4);
    painter->setPen(arcPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawArc(-radius + 2, -radius + 2, (radius - 2) * 2, (radius - 2) * 2,
                     static_cast<int>(GAUGE_START_ANGLE * 16),
                     static_cast<int>(GAUGE_SPAN_ANGLE * 16));

    
    // Draw red zone arc (AMG-style aggressive accent)
    float redZoneStartAngle = GAUGE_START_ANGLE + (RED_ZONE_START / MAX_SPEED) * GAUGE_SPAN_ANGLE;
    float redZoneSpan = ((MAX_SPEED - RED_ZONE_START) / MAX_SPEED) * GAUGE_SPAN_ANGLE;
    
    pen.setColor(QColor("#FF2D3E"));
    pen.setWidth(9);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    
    painter->drawArc(-radius + 10, -radius + 10, (radius - 10) * 2, (radius - 10) * 2,
                    static_cast<int>(redZoneStartAngle * 16),
                    static_cast<int>(redZoneSpan * 16));
    
    painter->restore();
}

void SpeedometerWidget::drawShiftLights(QPainter *painter)
{
    const int cx = width() / 2;
    const int cy = height() / 2;
    const int radius = qMin(width(), height()) / 2 - 20;
    constexpr int lightCount = 11;
    constexpr float startDeg = 200.0f;
    constexpr float endDeg = 340.0f;

    painter->save();
    painter->translate(cx, cy);

    const float normalized = qBound(0.0f, m_speed / MAX_SPEED, 1.0f);
    int activeLights = qBound(0, static_cast<int>(qCeil(normalized * lightCount)), lightCount);
    // Ensure red-zone entry clearly lights up the top-end bars.
    if (m_speed >= RED_ZONE_START) {
        activeLights = qMax(activeLights, lightCount - 2);
    }

    for (int i = 0; i < lightCount; ++i) {
        const float t = (lightCount == 1) ? 0.0f : static_cast<float>(i) / (lightCount - 1);
        const float angleDeg = startDeg + (endDeg - startDeg) * t;
        const float a = qDegreesToRadians(angleDeg);
        const float r = radius - 18.0f;
        const QPointF p(qCos(a) * r, qSin(a) * r);

        QColor color = QColor("#33445B");
        if (i < activeLights) {
            if (t < 0.55f) color = QColor("#B6F7FF");
            else if (t < 0.8f) color = QColor("#FFD65E");
            else color = QColor("#FF3848");
        }

        painter->setPen(Qt::NoPen);
        painter->setBrush(color);
        painter->drawRoundedRect(QRectF(p.x() - 7.0, p.y() - 1.9, 14.0, 3.8), 1.8, 1.8);
    }

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
        QPen pen(QColor("#EAF2FF"), 2);
        painter->setPen(pen);
        painter->drawLine(0, -radius + 15, 0, -radius + 35);
        
        // Speed label
        painter->rotate(-angle);  // Reset rotation for text
        float textAngle = angle * M_PI / 180.0f;
        int textX = static_cast<int>((radius - 55) * qCos(textAngle));
        int textY = static_cast<int>((radius - 55) * qSin(textAngle));
        
        QFont font("Roboto", 11, QFont::Medium);
        painter->setFont(font);
        painter->setPen(QColor("#C9D8EA"));
        
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
        
        QPen pen(QColor("#5E7088"), 1);
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
    
    // Porsche-like needle tip only: show only the outer segment.
    painter->setPen(QPen(needleColor, 4, Qt::SolidLine, Qt::RoundCap));
    const int innerTip = radius - 54;
    const int outerTip = radius - 24;
    painter->drawLine(QPointF(0, -innerTip), QPointF(0, -outerTip));
    
    painter->restore();
}

void SpeedometerWidget::drawDigitalSpeed(QPainter *painter)
{
    const int cx = width() / 2;
    const int cy = height() / 2 + 4;
    
    painter->save();
    
    // Compact center readout (minimal, no chunky rectangle).
    QFont unitFont("Roboto", 10, QFont::Medium);
    painter->setFont(unitFont);
    painter->setPen(QColor("#88A8C6"));
    QRect unitRect(cx - 74, cy + 24, 148, 14);
    painter->drawText(unitRect, Qt::AlignHCenter | Qt::AlignVCenter, "km/h");

    // Main speed number
    const QString speedText = QString::number(static_cast<int>(m_speed));
    QFont speedFont("Roboto Mono", 46, QFont::Bold);
    speedFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.8);
    painter->setFont(speedFont);

    QRect speedRect(cx - 96, cy - 30, 192, 60);
    painter->setPen(QColor(0, 0, 0, 105));
    painter->drawText(speedRect.adjusted(0, 2, 0, 2), Qt::AlignCenter, speedText);

    painter->setPen(QColor("#F3FBFF"));
    painter->drawText(speedRect, Qt::AlignCenter, speedText);
    
    painter->restore();
}
