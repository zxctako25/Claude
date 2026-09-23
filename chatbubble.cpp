#include "chatbubble.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QLayout>
#include <QRegularExpression>
#include <QApplication>
#include <QClipboard>
#include <QTimer>
#include <QDebug>

ChatBubble::ChatBubble(const QString &text, bool isUser, QWidget *parent)
    : QWidget(parent), m_isUser(isUser), m_rawText(text)
{
    setAttribute(Qt::WA_StyledBackground, true);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 12, 16, 12);
    root->setSpacing(0);

    m_contentLayout = new QVBoxLayout();
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_contentLayout->setSpacing(8);
    root->addLayout(m_contentLayout);

    if (isUser) {
        setStyleSheet(R"(
            ChatBubble {
                background: rgba(38, 38, 42, 0.55);
                border: 1px solid rgba(255, 255, 255, 0.10);
                border-radius: 16px;
            }
        )");
    } else {
        setStyleSheet(R"(
            ChatBubble {
                background: rgba(22, 22, 26, 0.50);
                border: 1px solid rgba(255, 255, 255, 0.07);
                border-radius: 16px;
            }
        )");
    }

    rebuild();
}

void ChatBubble::setText(const QString &text)
{
    m_rawText = text;
    rebuild();
}

void ChatBubble::appendText(const QString &chunk)
{
    m_rawText += chunk;
    rebuild();
}

void ChatBubble::clearLayout(QLayout *layout)
{
    if (!layout)
        return;
    while (QLayoutItem *item = layout->takeAt(0))
    {
        if (QWidget *w = item->widget())
        {
            w->hide();
            delete w;
        }
        else if (QLayout *sub = item->layout())
        {
            clearLayout(sub);
            delete sub;
        }
        delete item;
    }
}

QVector<ChatSegment> ChatBubble::parseSegments(const QString &md)
{
    QVector<ChatSegment> segs;
    QRegularExpression re("```(\\w*)\\n([\\s\\S]*?)```");
    int lastPos = 0;
    auto it = re.globalMatch(md);

    while (it.hasNext())
    {
        auto m = it.next();

        if (m.capturedStart() > lastPos)
        {
            ChatSegment s;
            s.kind = ChatSegment::Text;
            s.content = md.mid(lastPos, m.capturedStart() - lastPos);
            if (!s.content.trimmed().isEmpty())
                segs.append(s);
        }

        ChatSegment code;
        code.kind = ChatSegment::Code;
        code.language = m.captured(1);
        code.content = m.captured(2);
        segs.append(code);

        lastPos = m.capturedEnd();
    }

    if (lastPos < md.length())
    {
        ChatSegment s;
        s.kind = ChatSegment::Text;
        s.content = md.mid(lastPos);
        if (!s.content.trimmed().isEmpty())
            segs.append(s);
    }
    return segs;
}

void ChatBubble::rebuild()
{
    clearLayout(m_contentLayout);

    auto segments = parseSegments(m_rawText);
    if (segments.isEmpty())
    {
        auto *lbl = new QLabel(" ");
        lbl->setStyleSheet("background: transparent;");
        m_contentLayout->addWidget(lbl);
        return;
    }

    for (const auto &seg : segments)
    {
        if (seg.kind == ChatSegment::Text)
        {
            auto *lbl = new QLabel(seg.content);
            lbl->setWordWrap(true);
            lbl->setTextInteractionFlags(Qt::TextSelectableByMouse
                                         | Qt::LinksAccessibleByMouse);
            lbl->setOpenExternalLinks(true);
            lbl->setStyleSheet(
                "QLabel { color: #ECECEC; font-size: 14px;"
                "background: transparent; border: none; }");
            m_contentLayout->addWidget(lbl);
        }
        else
        {
            auto *codeFrame = new QFrame();
            codeFrame->setAttribute(Qt::WA_StyledBackground, true);
            codeFrame->setStyleSheet(R"(
                QFrame {
                    background: rgba(8, 8, 10, 0.85);
                    border: 1px solid rgba(255, 255, 255, 0.10);
                    border-radius: 12px;
                }
            )");

            auto *codeLayout = new QVBoxLayout(codeFrame);
            codeLayout->setContentsMargins(0, 0, 0, 0);
            codeLayout->setSpacing(0);

            auto *topBar = new QWidget();
            topBar->setAttribute(Qt::WA_StyledBackground, true);
            topBar->setStyleSheet(R"(
                QWidget {
                    background: rgba(255,255,255,0.03);
                    border-top-left-radius: 12px;
                    border-top-right-radius: 12px;
                    border-bottom: 1px solid rgba(255,255,255,0.06);
                }
            )");
            auto *topLayout = new QHBoxLayout(topBar);
            topLayout->setContentsMargins(14, 8, 8, 8);
            topLayout->setSpacing(8);

            auto *langLbl = new QLabel(
                seg.language.isEmpty() ? "code" : seg.language);
            langLbl->setStyleSheet(
                "color: #8A8A90; font-size: 11px;"
                "font-family: 'Consolas', 'Menlo', monospace;"
                "background: transparent; border: none;");

            auto *copyBtn = new QPushButton("📋  Копировать");
            copyBtn->setCursor(Qt::PointingHandCursor);
            copyBtn->setFixedHeight(26);
            copyBtn->setStyleSheet(R"(
                QPushButton {
                    background: rgba(255,255,255,0.06);
                    color: #B8B8BE;
                    border: 1px solid rgba(255,255,255,0.08);
                    border-radius: 6px;
                    padding: 0 12px;
                    font-size: 11px;
                }
                QPushButton:hover {
                    background: rgba(255,255,255,0.12);
                    color: #FFFFFF;
                }
                QPushButton:pressed {
                    background: rgba(255,255,255,0.18);
                }
            )");

            const QString codeCopy = seg.content;
            connect(copyBtn, &QPushButton::clicked, this, [copyBtn, codeCopy](){
                QApplication::clipboard()->setText(codeCopy);
                copyBtn->setText("✓ Скопировано");
                QTimer::singleShot(1500, copyBtn, [copyBtn](){
                    copyBtn->setText("Копировать");
                });
            });

            topLayout->addWidget(langLbl);
            topLayout->addStretch();
            topLayout->addWidget(copyBtn);

            auto *codeLbl = new QLabel(seg.content);
            codeLbl->setTextInteractionFlags(Qt::TextSelectableByMouse);
            codeLbl->setWordWrap(false);
            codeLbl->setStyleSheet(
                "QLabel { color: #D4D4D8;"
                "font-family: 'Consolas', 'Menlo', monospace;"
                "font-size: 13px; padding: 14px 16px;"
                "background: transparent; border: none; }");

            codeLayout->addWidget(topBar);
            codeLayout->addWidget(codeLbl);

            m_contentLayout->addWidget(codeFrame);
        }
    }
}
