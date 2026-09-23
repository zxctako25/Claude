#ifndef ANIMATEDBACKGROUND_H
#define ANIMATEDBACKGROUND_H

#include <QWidget>
#include <QTimer>

class AnimatedBackground : public QWidget
{
    Q_OBJECT
public:
    explicit AnimatedBackground(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onTick();

private:
    QTimer m_timer;
    qreal  m_phase = 0.0;
    qreal  m_pulse = 0.0;
};

#endif
