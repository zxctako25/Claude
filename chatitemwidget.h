#ifndef CHATITEMWIDGET_H
#define CHATITEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QToolButton>

class ChatItemWidget : public QWidget
{
    Q_OBJECT
public:
    QLabel *titleLabel;
    QToolButton *deleteBtn;

    explicit ChatItemWidget(const QString &title, int index,
                            QWidget *parent = nullptr);
    void setIndex(int index);
    void setTitle(const QString &title);
    int index() const
    {
        return m_index;
    }

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;

signals:
    void deleteRequested(int index);

private:
    int m_index;
};

#endif
