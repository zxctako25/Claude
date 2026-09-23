#include "animatedbackground.h"
#include <QPainter>
#include <QtMath>

AnimatedBackground::AnimatedBackground(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setAutoFillBackground(false);

    m_timer.setInterval(33);
    connect(&m_timer, &QTimer::timeout, this, &AnimatedBackground::onTick);
    m_timer.start();
}

void AnimatedBackground::onTick()
{
    m_phase += 1.0 / 1050.0;
    if (m_phase > 1.0)
        m_phase -= 1.0;

    m_pulse += 1.0 / 600.0;
    if (m_pulse > 1.0)
        m_pulse -= 1.0;
    update();
}

void AnimatedBackground::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    update();
}

void AnimatedBackground::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const int w = width();
    const int h = height();

    p.fillRect(rect(), QColor(0, 0, 0));

    {
        qreal angle = m_phase * 2.0 * M_PI;
        qreal cx = w * (0.5 + 0.35 * qCos(angle));
        qreal cy = h * (0.5 + 0.35 * qSin(angle));
        QRadialGradient g1(QPointF(cx, cy), qMax(w, h) * 0.7);
        g1.setColorAt(0.0, QColor(80, 80, 80, 100));
        g1.setColorAt(0.5, QColor(50, 50, 50, 60));
        g1.setColorAt(1.0, QColor(0, 0, 0, 0));
        p.fillRect(rect(), g1);
    }
    {
        qreal angle = -m_phase * 2.0 * M_PI + 1.2;
        qreal cx = w * (0.5 + 0.40 * qCos(angle));
        qreal cy = h * (0.5 + 0.40 * qSin(angle));
        QRadialGradient g2(QPointF(cx, cy), qMax(w, h) * 0.75);
        g2.setColorAt(0.0, QColor(90, 90, 90, 80));
        g2.setColorAt(0.6, QColor(40, 40, 40, 40));
        g2.setColorAt(1.0, QColor(0, 0, 0, 0));
        p.fillRect(rect(), g2);
    }
    {
        qreal scale = 0.9 + 0.25 * qSin(m_pulse * 2.0 * M_PI);
        QRadialGradient g3(QPointF(w * 0.25, h * 0.40), qMax(w, h) * 0.5 * scale);
        g3.setColorAt(0.0, QColor(70, 70, 70, 120));
        g3.setColorAt(1.0, QColor(0, 0, 0, 0));
        p.fillRect(rect(), g3);

        QRadialGradient g4(QPointF(w * 0.80, h * 0.60), qMax(w, h) * 0.45 * scale);
        g4.setColorAt(0.0, QColor(60, 60, 60, 100));
        g4.setColorAt(1.0, QColor(0, 0, 0, 0));
        p.fillRect(rect(), g4);

        QRadialGradient g5(QPointF(w * 0.50, h * 0.15), qMax(w, h) * 0.5 * scale);
        g5.setColorAt(0.0, QColor(50, 50, 50, 80));
        g5.setColorAt(1.0, QColor(0, 0, 0, 0));
        p.fillRect(rect(), g5);
    }
}
