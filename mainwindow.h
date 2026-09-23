#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QNetworkAccessManager>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QListWidget>
#include <QPropertyAnimation>
#include <QVector>
#include <QJsonArray>
#include <QFrame>
#include <QNetworkReply>

struct Msg
{
    QString role;
    QString text;
};

struct ModelEntry
{
    QString id;
    QString display;
    QString family;
};

class ChatBubble;
class AnimatedBackground;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;
    void resizeEvent(QResizeEvent *e) override;
    void closeEvent(QCloseEvent *e) override;

private slots:
    void newChat();
    void sendMessage();
    void cancelStream();
    void switchChat(int index);
    void toggleSidebar();
    void deleteChatAt(int index);
    void openModelSelector();
    void onStreamReadyRead();
    void onStreamFinished();
    void adjustPromptHeight();

private:
    void addBubble(const QString &text, bool isUser);
    void showGreeting(bool show);
    void rebuildChatView();
    QString chatTitle(const QString &firstMsg);
    void updateChatListItem(int index);
    void scrollToBottom();
    void fetchModels();
    void setStreaming(bool on);
    void buildUI();
    void clearChatLayout();
    void buildSidebar(QHBoxLayout *root);
    void buildMainPanel(QHBoxLayout *root);

    void saveChats();
    void loadChats();
    QString storagePath() const;

    QString apiKey  = "sk-hub";
    QString baseUrl = "https://api.claudehub.fun";
    QString model   = "claude-opus-5";
    QString modelDisplayName = "Claude Opus 5";

    QNetworkAccessManager *network = nullptr;
    QNetworkReply *streamReply = nullptr;
    QByteArray    sseBuffer;
    ChatBubble   *activeBubble = nullptr;

    QVector<QVector<Msg>> chats;
    int currentChat = -1;
    bool sidebarVisible = true;
    bool isStreaming = false;
    bool m_rebuilding = false;

    QVector<ModelEntry> availableModels;

    AnimatedBackground *bg = nullptr;

    QWidget *sidebar = nullptr;
    QListWidget *chatList = nullptr;
    QVBoxLayout *chatLayout = nullptr;
    QScrollArea *scrollArea = nullptr;
    QWidget *chatContainer = nullptr;
    QWidget *greeting = nullptr;
    QPlainTextEdit *promptEntry = nullptr;
    QPushButton *sendBtn = nullptr;
    QPushButton *toggleBtn = nullptr;
    QPushButton *newChatBtn = nullptr;
    QLabel *inputModelLabel = nullptr;
    QPushButton *menuBtn = nullptr;
    QLabel *typingLabel = nullptr;
    QTimer *typingTimer = nullptr;
    QPropertyAnimation *sidebarAnim = nullptr;
    int typingDots = 0;
};

#endif
