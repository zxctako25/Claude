#ifndef CHATBUBBLE_H
#define CHATBUBBLE_H

#include <QWidget>
#include <QString>
#include <QVector>

class QLabel;
class QVBoxLayout;

struct ChatSegment {
    enum Kind { Text, Code };
    Kind kind;
    QString language;
    QString content;
};

class ChatBubble : public QWidget
{
    Q_OBJECT
public:
    ChatBubble(const QString &text, bool isUser, QWidget *parent = nullptr);

    QString text() const
    {
        return m_rawText;
    }

    void appendText(const QString &chunk);
    void setText(const QString &text);

private:
    bool m_isUser;
    QString m_rawText;
    QVBoxLayout *m_contentLayout = nullptr;

    void rebuild();
    void clearLayout(QLayout *layout);
    static QVector<ChatSegment> parseSegments(const QString &md);
};

#endif
