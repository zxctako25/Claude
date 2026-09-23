#include "chatitemwidget.h"
#include <QHBoxLayout>
#include <QSvgRenderer>
#include <QPainter>
#include <QPixmap>
#include <QEvent>
#include <QDebug>

static const char *kTrashSvgTemplate = R"(
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512">
  <g fill="%1">
    <path d="M356.65,450H171.47a41,41,0,0,1-40.9-40.9V120.66a15,15,0,0,1,15-15h237a15,15,0,0,1,15,15V409.1A41,41,0,0,1,356.65,450ZM160.57,135.66V409.1a10.91,10.91,0,0,0,10.9,10.9H356.65a10.91,10.91,0,0,0,10.91-10.9V135.66Z"/>
    <path d="M327.06,135.66h-126a15,15,0,0,1-15-15V93.4A44.79,44.79,0,0,1,230.8,48.67h66.52A44.79,44.79,0,0,1,342.06,93.4v27.26A15,15,0,0,1,327.06,135.66Zm-111-30h96V93.4a14.75,14.75,0,0,0-14.74-14.73H230.8A14.75,14.75,0,0,0,216.07,93.4Z"/>
    <path d="M264.06,392.58a15,15,0,0,1-15-15V178.09a15,15,0,1,1,30,0V377.58A15,15,0,0,1,264.06,392.58Z"/>
    <path d="M209.9,392.58a15,15,0,0,1-15-15V178.09a15,15,0,0,1,30,0V377.58A15,15,0,0,1,209.9,392.58Z"/>
    <path d="M318.23,392.58a15,15,0,0,1-15-15V178.09a15,15,0,0,1,30,0V377.58A15,15,0,0,1,318.23,392.58Z"/>
    <path d="M405.81,135.66H122.32a15,15,0,0,1,0-30H405.81a15,15,0,0,1,0,30Z"/>
  </g>
</svg>)";

static QPixmap renderTrashIcon(int size, const QColor &color)
{
    const int ss = 4;
    const int bigSize = size * ss;

    QImage big(bigSize, bigSize, QImage::Format_ARGB32_Premultiplied);
    big.fill(Qt::transparent);

    QString svg = QString::fromUtf8(kTrashSvgTemplate);
    svg.replace("%1", color.name(QColor::HexRgb));

    QSvgRenderer r(svg.toUtf8());
    if (r.isValid())
    {
        QPainter p(&big);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setRenderHint(QPainter::SmoothPixmapTransform, true);
        r.render(&p);
    }

    QImage small = big.scaled(size, size,
                              Qt::KeepAspectRatio,
                              Qt::SmoothTransformation);
    return QPixmap::fromImage(small);
}

ChatItemWidget::ChatItemWidget(const QString &title, int index, QWidget *parent)
    : QWidget(parent), m_index(index)
{
    setAttribute(Qt::WA_StyledBackground, true);

    auto *h = new QHBoxLayout(this);
    h->setContentsMargins(12, 6, 6, 6);
    h->setSpacing(6);

    titleLabel = new QLabel(title, this);
    titleLabel->setStyleSheet(
        "color: #B8B8BE; font-size: 13px; background: transparent; border: none;");
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    deleteBtn = new QToolButton(this);
    deleteBtn->setCursor(Qt::PointingHandCursor);
    deleteBtn->setFixedSize(26, 26);
    deleteBtn->setIconSize(QSize(14, 14));
    deleteBtn->setIcon(QIcon(renderTrashIcon(14, QColor("#6E6E75"))));
    deleteBtn->setToolTip("Удалить чат");

    deleteBtn->setStyleSheet(R"(
        QToolButton {
            background: transparent;
            border: none;
            border-radius: 8px;
            padding: 0;
        }
        QToolButton:hover {
            background: rgba(229, 115, 115, 0.12);
        }
        QToolButton:pressed {
            background: rgba(229, 115, 115, 0.22);
        }
    )");

    deleteBtn->installEventFilter(this);

    h->addWidget(titleLabel, 1);
    h->addWidget(deleteBtn, 0);

    connect(deleteBtn, &QToolButton::clicked, this, [this]() {
        emit deleteRequested(m_index);
    });
}

bool ChatItemWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == deleteBtn) {
        if (ev->type() == QEvent::Enter)
        {
            deleteBtn->setIcon(QIcon(renderTrashIcon(14, QColor("#E57373"))));
        }
        else if (ev->type() == QEvent::Leave)
        {
            deleteBtn->setIcon(QIcon(renderTrashIcon(14, QColor("#6E6E75"))));
        }
    }
    return QWidget::eventFilter(obj, ev);
}

void ChatItemWidget::setIndex(int index)
{
    m_index = index;
}

void ChatItemWidget::setTitle(const QString &title)
{
    titleLabel->setText(title);
}
