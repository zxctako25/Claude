#include "mainwindow.h"
#include "animatedbackground.h"
#include "chatbubble.h"
#include "chatitemwidget.h"
#include "modelselectordialog.h"

#include <QScrollBar>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QHBoxLayout>
#include <QDialog>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QtMath>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <algorithm>

QString MainWindow::storagePath() const
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir + "/chats.json";
}

void MainWindow::saveChats()
{
    QJsonObject root;
    root["currentChat"] = currentChat;
    root["model"] = model;
    root["modelDisplayName"] = modelDisplayName;

    QJsonArray chatsArr;
    for (const auto &chat : chats)
    {
        QJsonArray msgsArr;
        for (const auto &m : chat)
        {
            QJsonObject mo;
            mo["role"] = m.role;
            mo["text"] = m.text;
            msgsArr.append(mo);
        }
        chatsArr.append(msgsArr);
    }
    root["chats"] = chatsArr;

    QSaveFile f(storagePath());
    if (!f.open(QIODevice::WriteOnly))
    {
        qWarning() << "[saveChats] cannot open" << storagePath();
        return;
    }
    f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    f.commit();
    qInfo() << "[saveChats] saved" << chats.size() << "chats";
}

void MainWindow::loadChats()
{
    QFile f(storagePath());
    if (!f.exists())
    {
        qInfo() << "[loadChats] no file yet";
        return;
    }
    if (!f.open(QIODevice::ReadOnly))
    {
        qWarning() << "[loadChats] cannot open";
        return;
    }

    auto doc = QJsonDocument::fromJson(f.readAll());
    auto root = doc.object();

    chats.clear();
    auto chatsArr = root.value("chats").toArray();
    for (const auto &cv : chatsArr)
    {
        QVector<Msg> chat;
        auto msgsArr = cv.toArray();
        for (const auto &mv : msgsArr)
        {
            auto mo = mv.toObject();
            Msg m;
            m.role = mo.value("role").toString();
            m.text = mo.value("text").toString();
            chat.append(m);
        }
        chats.append(chat);
    }

    qInfo() << "[loadChats] loaded" << chats.size() << "chats";

    for (int i = 0; i < chats.size(); ++i)
    {
        QString title = "Новый чат";
        for (const Msg &m : chats[i])
        {
            if (m.role == "user")
            {
                title = chatTitle(m.text);
                break;
            }
        }

        auto *item = new QListWidgetItem(chatList);
        auto *widget = new ChatItemWidget(title, i);
        item->setSizeHint(QSize(0, 34));
        connect(widget, &ChatItemWidget::deleteRequested,
                this, &MainWindow::deleteChatAt);
        chatList->setItemWidget(item, widget);
    }
    currentChat = -1;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Claude");
    network = new QNetworkAccessManager(this);

    setStyleSheet(R"(
        QMainWindow { background: #000000; }
        QWidget { font-family: 'Inter', 'Segoe UI', system-ui, sans-serif; font-size: 14px; }
        QToolTip { background: #1A1A1E; color: #ECECEC; border: 1px solid #2A2A2E;
                   padding: 6px 10px; border-radius: 6px; }
    )");

    buildUI();

    sidebarAnim = new QPropertyAnimation(sidebar, "maximumWidth", this);
    sidebarAnim->setDuration(220);
    sidebarAnim->setEasingCurve(QEasingCurve::InOutCubic);

    connect(newChatBtn, &QPushButton::clicked, this, &MainWindow::newChat);
    connect(sendBtn,    &QPushButton::clicked, this, &MainWindow::sendMessage);
    connect(chatList,   &QListWidget::currentRowChanged, this, &MainWindow::switchChat);

    connect(toggleBtn, &QPushButton::clicked, this, [this](){
        toggleSidebar();
        menuBtn->setVisible(!sidebarVisible);
    });
    connect(menuBtn, &QPushButton::clicked, this, [this](){
        toggleSidebar();
        menuBtn->setVisible(!sidebarVisible);
    });

    promptEntry->installEventFilter(this);
    inputModelLabel->installEventFilter(this);

    connect(promptEntry, &QPlainTextEdit::textChanged,
            this, &MainWindow::adjustPromptHeight);

    loadChats();
    newChat();
    fetchModels();

    showMaximized();
}

MainWindow::~MainWindow() = default;

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    if (bg)
    {
        bg->setGeometry(centralWidget()->rect());
        bg->lower();
    }
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    saveChats();
    QMainWindow::closeEvent(e);
}

void MainWindow::buildUI()
{
    auto *central = new QWidget(this);
    setCentralWidget(central);

    bg = new AnimatedBackground(central);
    bg->setGeometry(central->rect());
    bg->lower();

    auto *root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    buildSidebar(root);
    buildMainPanel(root);
}

void MainWindow::buildSidebar(QHBoxLayout *root)
{
    sidebar = new QWidget();
    sidebar->setMinimumWidth(0);
    sidebar->setMaximumWidth(264);
    sidebar->setStyleSheet(
        "background: rgba(0, 0, 0, 0.4);"
        "border-right: 1px solid rgba(255, 255, 255, 0.06);");

    auto *sl = new QVBoxLayout(sidebar);
    sl->setContentsMargins(14, 16, 14, 14);
    sl->setSpacing(10);

    auto *topBar = new QHBoxLayout();
    topBar->setSpacing(6);

    auto *logo = new QLabel("Claude");
    logo->setStyleSheet("color: #FFFFFF; font-size: 15px; font-weight: 700; padding-left: 6px;");
    topBar->addWidget(logo);
    topBar->addStretch();

    toggleBtn = new QPushButton("‹");
    toggleBtn->setFixedSize(30, 30);
    toggleBtn->setCursor(Qt::PointingHandCursor);
    toggleBtn->setToolTip("Свернуть панель");
    toggleBtn->setStyleSheet(R"(
        QPushButton {
            background: rgba(255,255,255,0.04);
            border: 1px solid rgba(255,255,255,0.08);
            border-radius: 15px;
            color: #B8B8BE;
            font-size: 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: rgba(255,255,255,0.10);
            border: 1px solid rgba(255,255,255,0.15);
            color: #FFFFFF;
        }
        QPushButton:pressed {
            background: rgba(255,255,255,0.15);
        }
    )");
    topBar->addWidget(toggleBtn);
    sl->addLayout(topBar);
    sl->addSpacing(6);

    newChatBtn = new QPushButton("Новый чат");
    newChatBtn->setCursor(Qt::PointingHandCursor);
    newChatBtn->setFixedHeight(38);
    newChatBtn->setStyleSheet(R"(
        QPushButton { background: #FFFFFF; color: #0A0A0A; border: none;
                      border-radius: 10px; font-weight: 600; font-size: 13px; }
        QPushButton:hover { background: #E8E8EA; }
        QPushButton:pressed { background: #D5D5D8; }
    )");
    sl->addWidget(newChatBtn);

    chatList = new QListWidget();
    chatList->setStyleSheet(R"(
        QListWidget { background: transparent; border: none; outline: none; }
        QListWidget::item { background: transparent; border: none; padding: 0; margin: 1px 0; }
        QListWidget::item:selected { background: transparent; }
    )");
    chatList->setSpacing(0);
    chatList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    sl->addWidget(chatList, 1);

    root->addWidget(sidebar);
}

void MainWindow::buildMainPanel(QHBoxLayout *root)
{
    auto *main = new QWidget();
    main->setStyleSheet("background: transparent;");
    auto *ml = new QVBoxLayout(main);
    ml->setContentsMargins(0, 0, 0, 0);
    ml->setSpacing(0);

    auto *topPanel = new QWidget();
    topPanel->setFixedHeight(50);
    topPanel->setStyleSheet("background: transparent; border-bottom: 1px solid rgba(255,255,255,0.06);");
    auto *tpl = new QHBoxLayout(topPanel);
    tpl->setContentsMargins(16, 0, 16, 0);

    menuBtn = new QPushButton("☰");
    menuBtn->setFixedSize(28, 28);
    menuBtn->setCursor(Qt::PointingHandCursor);
    menuBtn->setStyleSheet(R"(
        QPushButton { background: transparent; color: #8A8A90; border: none;
                      border-radius: 6px; font-size: 13px; }
        QPushButton:hover { background: rgba(255,255,255,0.08); color: #FFFFFF; }
    )");
    menuBtn->hide();

    tpl->addWidget(menuBtn);
    tpl->addStretch();
    ml->addWidget(topPanel);

    greeting = new QWidget();
    auto *gl = new QVBoxLayout(greeting);
    gl->setAlignment(Qt::AlignCenter);
    auto *gTitle = new QLabel("Чем могу помочь?");
    gTitle->setAlignment(Qt::AlignCenter);
    gTitle->setStyleSheet("color: #FFFFFF; font-size: 30px; font-weight: 600;");
    auto *gSub = new QLabel("Задайте вопрос — модель ответит в потоковом режиме.");
    gSub->setAlignment(Qt::AlignCenter);
    gSub->setStyleSheet("color: #8A8A90; font-size: 14px;");
    gl->addWidget(gTitle);
    gl->addSpacing(12);
    gl->addWidget(gSub);

    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setAttribute(Qt::WA_TranslucentBackground, true);
    scrollArea->viewport()->setAutoFillBackground(false);
    scrollArea->viewport()->setStyleSheet("background: transparent;");

    chatContainer = new QWidget();
    chatContainer->setStyleSheet("background: transparent;");
    chatLayout = new QVBoxLayout(chatContainer);
    chatLayout->setAlignment(Qt::AlignTop);
    chatLayout->setSpacing(16);
    chatLayout->setContentsMargins(0, 32, 0, 32);
    scrollArea->setWidget(chatContainer);
    scrollArea->setStyleSheet(R"(
        QScrollArea { background: transparent; border: none; }
        QScrollBar:vertical { background: transparent; width: 8px; margin: 4px 2px; }
        QScrollBar::handle:vertical { background: rgba(255,255,255,0.12);
                                      border-radius: 4px; min-height: 32px; }
        QScrollBar::handle:vertical:hover { background: rgba(255,255,255,0.2); }
        QScrollBar::add-line, QScrollBar::sub-line { height: 0; }
        QScrollBar::add-page, QScrollBar::sub-page { background: transparent; }
    )");

    auto *stack = new QWidget();
    stack->setStyleSheet("background: transparent;");
    auto *stackL = new QVBoxLayout(stack);
    stackL->setContentsMargins(0, 0, 0, 0);
    stackL->setSpacing(0);
    stackL->addWidget(greeting, 1);
    stackL->addWidget(scrollArea, 1);
    scrollArea->hide();
    ml->addWidget(stack, 1);

    typingLabel = new QLabel("Claude печатает");
    typingLabel->setStyleSheet("color: #8A8A90; font-size: 12px; padding: 0 40px 6px 40px;");
    typingLabel->hide();
    typingTimer = new QTimer(this);
    connect(typingTimer, &QTimer::timeout, this, [this](){
        typingDots = (typingDots + 1) % 4;
        typingLabel->setText("Claude печатает" + QString(".").repeated(typingDots));
    });
    ml->addWidget(typingLabel);

    auto *inputCard = new QWidget();
    inputCard->setObjectName("inputCard");
    inputCard->setAttribute(Qt::WA_StyledBackground, true);
    inputCard->setStyleSheet(R"(
        #inputCard { background: rgba(10, 10, 10, 0.6);
                     border: 1px solid rgba(255, 255, 255, 0.08);
                     border-radius: 20px; }
    )");
    auto *cl = new QVBoxLayout(inputCard);
    cl->setContentsMargins(18, 14, 14, 12);
    cl->setSpacing(8);

    promptEntry = new QPlainTextEdit();
    promptEntry->setPlaceholderText("Введите сообщение...");
    promptEntry->setFixedHeight(42);
    promptEntry->setFrameStyle(0);
    promptEntry->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    promptEntry->setStyleSheet(R"(
        QPlainTextEdit { background: transparent; color: #ECECEC; border: none;
                         font-size: 15px; padding: 4px; }
        QScrollBar:vertical { background: transparent; width: 6px; margin: 2px; }
        QScrollBar::handle:vertical { background: rgba(255,255,255,0.15);
                                      border-radius: 3px; min-height: 20px; }
        QScrollBar::add-line, QScrollBar::sub-line { height: 0; }
        QScrollBar::add-page, QScrollBar::sub-page { background: transparent; }
    )");
    cl->addWidget(promptEntry);

    auto *cb = new QHBoxLayout();
    inputModelLabel = new QLabel(modelDisplayName);
    inputModelLabel->setStyleSheet("color: #8A8A90; font-size: 13px; padding: 6px 10px;");
    inputModelLabel->setCursor(Qt::PointingHandCursor);

    sendBtn = new QPushButton("↑");
    sendBtn->setCursor(Qt::PointingHandCursor);
    sendBtn->setFixedSize(34, 34);
    sendBtn->setStyleSheet(R"(
        QPushButton { background: #FFFFFF; color: #0A0A0A; border: none;
                      border-radius: 17px; font-size: 15px; font-weight: 700; }
        QPushButton:hover { background: #E8E8EA; }
        QPushButton:disabled { background: rgba(255,255,255,0.08); color: #4A4A51; }
    )");
    cb->addStretch();
    cb->addWidget(inputModelLabel);
    cb->addWidget(sendBtn);
    cl->addLayout(cb);

    auto *iw = new QHBoxLayout();
    iw->setContentsMargins(40, 0, 40, 30);
    iw->addWidget(inputCard, 1);
    ml->addLayout(iw);

    root->addWidget(main, 1);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == promptEntry && ev->type() == QEvent::KeyPress)
    {
        auto *ke = static_cast<QKeyEvent*>(ev);
        if ((ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter)
            && !(ke->modifiers() & Qt::ShiftModifier))
        {
            sendMessage();
            return true;
        }
    }
    if (obj == inputModelLabel && ev->type() == QEvent::MouseButtonPress)
    {
        openModelSelector();
        return true;
    }
    return QMainWindow::eventFilter(obj, ev);
}

void MainWindow::adjustPromptHeight()
{
    if (!promptEntry)
        return;
    int docH = qCeil(promptEntry->document()->size().height()) + 20;
    int newH = qBound(42, docH, 180);
    if (promptEntry->height() != newH)
        promptEntry->setFixedHeight(newH);

    auto *sb = promptEntry->verticalScrollBar();
    if (sb)
        sb->setValue(sb->maximum());
}

void MainWindow::scrollToBottom()
{
    QTimer::singleShot(0, this, [this]()
    {
        if (!scrollArea)
            return;

        auto *sb = scrollArea->verticalScrollBar();
        if (sb)
            sb->setValue(sb->maximum());
    });
}

void MainWindow::toggleSidebar()
{
    if (!sidebarAnim || !sidebar || !toggleBtn)
        return;
    sidebarVisible = !sidebarVisible;
    sidebarAnim->stop();
    sidebarAnim->setStartValue(sidebar->maximumWidth());
    sidebarAnim->setEndValue(sidebarVisible ? 264 : 0);
    sidebarAnim->start();
    toggleBtn->setText(sidebarVisible ? "‹" : "›");
}

QString MainWindow::chatTitle(const QString &firstMsg) {
    QString t = firstMsg.simplified();
    return t.length() > 30 ? t.left(30) + "…" : t;
}

void MainWindow::newChat()
{
    if (currentChat >= 0 && currentChat < chats.size()
        && chats[currentChat].isEmpty())
    {
        rebuildChatView();
        promptEntry->setFocus();
        return;
    }

    for (int i = 0; i < chats.size(); ++i)
    {
        if (chats[i].isEmpty())
        {
            currentChat = i;
            m_rebuilding = true;
            chatList->setCurrentRow(i);
            m_rebuilding = false;
            rebuildChatView();
            promptEntry->setFocus();
            return;
        }
    }

    QVector<Msg> emptyChat;
    chats.prepend(emptyChat);
    currentChat = 0;

    auto *item = new QListWidgetItem();
    auto *widget = new ChatItemWidget("Новый чат", 0);
    item->setSizeHint(QSize(0, 34));
    chatList->insertItem(0, item);

    connect(widget, &ChatItemWidget::deleteRequested,
            this, &MainWindow::deleteChatAt);

    chatList->setItemWidget(item, widget);

    for (int i = 0; i < chatList->count(); ++i)
    {
        auto *w = qobject_cast<ChatItemWidget*>(
            chatList->itemWidget(chatList->item(i)));
        if (w)
            w->setIndex(i);
    }

    m_rebuilding = true;
    chatList->setCurrentRow(0);
    m_rebuilding = false;

    rebuildChatView();
    promptEntry->setFocus();
}

void MainWindow::switchChat(int index)
{
    if (m_rebuilding)
        return;
    if (index < 0 || index >= chats.size())
        return;
    currentChat = index;
    rebuildChatView();
}

void MainWindow::deleteChatAt(int index)
{
    if (index < 0 || index >= chats.size())
        return;

    chats.removeAt(index);
    QListWidgetItem *item = chatList->takeItem(index);
    delete item;

    for (int i = 0; i < chatList->count(); ++i)
    {
        auto *it = chatList->item(i);
        auto *w = qobject_cast<ChatItemWidget*>(chatList->itemWidget(it));
        if (w) w->setIndex(i);
    }

    if (chats.isEmpty())
    {
        currentChat = -1;
        newChat();
        return;
    }

    currentChat = qBound(0, index, chats.size() - 1);
    m_rebuilding = true;
    chatList->setCurrentRow(currentChat);
    m_rebuilding = false;
    rebuildChatView();
    saveChats();
}

void MainWindow::updateChatListItem(int index)
{
    if (index < 0 || index >= chatList->count() || chats[index].isEmpty())
        return;
    for (const Msg &m : chats[index])
    {
        if (m.role == "user")
        {
            auto *item = chatList->item(index);
            if (!item)
                return;
            auto *w = qobject_cast<ChatItemWidget*>(chatList->itemWidget(item));
            if (w)
                w->setTitle(chatTitle(m.text));
            break;
        }
    }
}

void MainWindow::showGreeting(bool show)
{
    if (greeting)
        greeting->setVisible(show);
    if (scrollArea)
        scrollArea->setVisible(!show);
}

void MainWindow::clearChatLayout()
{
    if (!chatLayout)
        return;
    while (QLayoutItem *item = chatLayout->takeAt(0))
    {
        if (QWidget *w = item->widget())
        {
            w->hide();
            delete w;
        }
        delete item;
    }
}

void MainWindow::rebuildChatView()
{
    clearChatLayout();
    if (currentChat < 0 || currentChat >= chats.size() || chats[currentChat].isEmpty())
    {
        showGreeting(true);
        return;
    }
    showGreeting(false);
    for (const Msg &m : chats[currentChat])
        addBubble(m.text, m.role == "user");
    scrollToBottom();
}

void MainWindow::addBubble(const QString &text, bool isUser)
{
    auto *bubble = new ChatBubble(text, isUser);

    auto *wrap = new QWidget();
    wrap->setStyleSheet("background: transparent;");

    auto *h = new QHBoxLayout(wrap);
    h->setContentsMargins(40, 0, 40, 0);
    h->setSpacing(0);

    if (isUser)
    {
        h->addStretch(1);
        bubble->setMaximumWidth(700);
        bubble->setMinimumWidth(120);
        h->addWidget(bubble, 3, Qt::AlignTop | Qt::AlignRight);
    }
    else
    {
        bubble->setMaximumWidth(1000);
        bubble->setMinimumWidth(200);
        h->addWidget(bubble, 5, Qt::AlignTop | Qt::AlignLeft);
        h->addStretch(1);
    }
    chatLayout->addWidget(wrap);
}

void MainWindow::setStreaming(bool on)
{
    isStreaming = on;
    if (!sendBtn)
        return;

    sendBtn->setText(on ? "■" : "↑");
    sendBtn->setToolTip(on ? "Остановить генерацию" : "Отправить (Enter)");
    sendBtn->setStyleSheet(on
                               ? R"(QPushButton { background: #FFFFFF; color: #0A0A0A; border: none;
                            border-radius: 17px; font-size: 11px; font-weight: 700; }
             QPushButton:hover { background: #E8E8EA; })"
                               : R"(QPushButton { background: #FFFFFF; color: #0A0A0A; border: none;
                            border-radius: 17px; font-size: 15px; font-weight: 700; }
             QPushButton:hover { background: #E8E8EA; }
             QPushButton:disabled { background: rgba(255,255,255,0.08); color: #4A4A51; })");
}

void MainWindow::fetchModels()
{
    QNetworkRequest req(QUrl(baseUrl + "/v1/models"));
    auto *reply = network->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]()
    {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError)
        {
            qWarning() << "[fetchModels] error:" << reply->errorString();
            return;
        }

        auto doc = QJsonDocument::fromJson(reply->readAll());
        auto arr = doc.object().value("data").toArray();

        availableModels.clear();
        for (const auto &v : arr)
        {
            auto o = v.toObject();
            QString id = o.value("id").toString();
            if (id.isEmpty())
                continue;

            QString low = id.toLower();
            QString fam;

            if (low.contains("opus"))
                fam = "Opus";
            else if (low.contains("sonnet"))
                fam = "Sonnet";
            else if (low.contains("haiku"))
                fam = "Haiku";
            else if (low.contains("fable"))
                fam = "Fable";
            else if (low.contains("gpt"))
                fam = "GPT";
            else if (low.contains("gemini"))
                fam = "Gemini";
            else if (low.contains("deepseek"))
                fam = "DeepSeek";
            else if (low.contains("glm"))
                fam = "GLM";
            else if (low.contains("grok"))
                fam = "Grok";
            else if (low.contains("kimi"))
                fam = "Kimi";
            else if (low.contains("qwen"))
                fam = "Qwen";
            else
                continue;

            QString disp = o.value("display_name").toString();
            if (disp.isEmpty())
                disp = id;
            availableModels.append({id, disp, fam});
        }
        qInfo() << "[fetchModels] loaded" << availableModels.size() << "models";
    });
}

void MainWindow::openModelSelector() {
    if (availableModels.isEmpty())
    {
        availableModels = {
                            {
                "claude-opus-4.8",
             "Claude Opus 4.8",
             "Opus"
            },
                            {
              "claude-opus-4.7",
             "Claude Opus 4.7",
             "Opus"
            },
                            {
              "claude-opus-4.6",
             "Claude Opus 4.6",
             "Opus"
            },
                            {
              "claude-opus-5",
             "Claude Opus 5",
             "Opus"
            },
                            {
              "claude-sonnet-4.6",
             "Claude Sonnet 4.6",
             "Sonnet"
            },
                            {
              "claude-sonnet-5",
             "Claude Sonnet 5",
             "Sonnet"
            },
                            {
              "claude-haiku-4.5",
             "Claude Haiku 4.5",
             "Haiku"
            },
                            {
              "claude-fable-5",
             "Claude Fable 5",
             "Fable"
            },
                            {
              "gpt-5.4",
             "GPT 5.4",
             "GPT"
            },
                            {
              "gpt-5.5",
             "GPT 5.5",
             "GPT"
            },
                            {
              "gpt-5.6-luna",
             "GPT-5.6 Luna",
             "GPT"
            },
                            {
              "gpt-5.6-sol",
             "GPT-5.6 Sol",
             "GPT"
            },
                            {
              "gpt-5.6-terra",
             "GPT-5.6 Terra",
             "GPT"
            },
                            {
              "gpt-6-astra",
             "GPT-6 Astra",
             "GPT"
            },
                            {
              "deepseek-v4-flash",
             "DeepSeek V4 Flash",
             "DeepSeek"
            },
                            {
              "deepseek-v4-pro",
             "DeepSeek V4 Pro",
             "DeepSeek"
            },
                            {
              "deepseek-v4.1-flash",
             "Deepseek V4.1 Flash 0910",
             "DeepSeek"
            },
                            {
              "gemini-3.1-flash-image",
             "Gemini 3.1 Flash Image",
             "Gemini"
            },
                            {
              "glm-5.3",
             "GLM 5.3",
             "GLM"
            },
                            {
              "glm-5.3-flash",
             "GLM 5.3 Flash",
             "GLM"
            },
                            {
              "grok-4.6",
             "Grok 4.6",
             "Grok"
            },
                            {
              "grok-4.7",
             "Grok 4.7",
             "Grok"
            },
                            {
              "kimi-2.7",
             "kimi 2.7",
             "Kimi"
            },
                            {
              "kimi-k3",
             "Kimi K3",
             "Kimi"
            },
                            {
              "Qwen-3.8-Max",
             "Qwen-3.8 Max",
             "Qwen"
            },
                            };
    }

    ModelSelectorDialog dlg(availableModels, model, this);
    if (dlg.exec() == QDialog::Accepted && !dlg.selectedModel.isEmpty())
    {
        model = dlg.selectedModel;
        modelDisplayName = dlg.selectedName;
        inputModelLabel->setText(modelDisplayName);
        saveChats();
    }
}

void MainWindow::sendMessage()
{
    if (isStreaming)
    {
        cancelStream();
        return;
    }

    QString text = promptEntry->toPlainText().trimmed();
    if (text.isEmpty() || currentChat < 0)
        return;

    promptEntry->clear();
    adjustPromptHeight();
    showGreeting(false);

    chats[currentChat].append({"user", text});
    addBubble(text, true);
    updateChatListItem(currentChat);
    scrollToBottom();
    saveChats();

    setStreaming(true);
    typingDots = 0;
    typingLabel->setText("Claude печатает");
    typingLabel->show();
    typingTimer->start(380);

    QJsonArray messages;
    for (const Msg &m : chats[currentChat])
        messages.append(QJsonObject{{"role", m.role}, {"content", m.text}});

    QJsonObject body
    {
        {
            "model",
            model
        },
        {
            "max_tokens",
            4096
        },
        {
            "stream",
            true
        },
        {
            "messages",
            messages
        }
    };

    QNetworkRequest req(QUrl(baseUrl + "/v1/messages"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("x-api-key", apiKey.toUtf8());
    req.setRawHeader("anthropic-version", "2023-06-01");
    req.setRawHeader("Accept", "text/event-stream");
    req.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                     QNetworkRequest::NoLessSafeRedirectPolicy);

    streamReply = network->post(req, QJsonDocument(body).toJson());
    sseBuffer.clear();

    activeBubble = new ChatBubble("", false);
    auto *wrap = new QWidget();
    wrap->setStyleSheet("background: transparent;");
    auto *h = new QHBoxLayout(wrap);
    h->setContentsMargins(40, 0, 40, 0);
    h->setSpacing(0);
    h->addWidget(activeBubble, 5, Qt::AlignTop | Qt::AlignLeft);
    h->addStretch(1);
    activeBubble->setMaximumWidth(1000);
    activeBubble->setMinimumWidth(200);
    chatLayout->addWidget(wrap);

    connect(streamReply, &QNetworkReply::readyRead, this, &MainWindow::onStreamReadyRead);
    connect(streamReply, &QNetworkReply::finished,  this, &MainWindow::onStreamFinished);
}

void MainWindow::onStreamReadyRead()
{
    if (!streamReply)
        return;
    sseBuffer.append(streamReply->readAll());

    int idx;
    while ((idx = sseBuffer.indexOf("\n\n")) != -1)
    {
        QByteArray raw = sseBuffer.left(idx);
        sseBuffer.remove(0, idx + 2);

        QByteArray dataLine;
        for (const QByteArray &line : raw.split('\n'))
        {
            if (line.startsWith("data:"))
            {
                QByteArray chunk = line.mid(5).trimmed();
                if (!dataLine.isEmpty())
                    dataLine.append('\n');
                dataLine.append(chunk);
            }
        }
        if (dataLine.isEmpty())
            continue;

        QJsonParseError err;
        auto doc = QJsonDocument::fromJson(dataLine, &err);
        if (err.error != QJsonParseError::NoError)
            continue;

        auto obj = doc.object();
        QString type = obj.value("type").toString();

        if (type == "content_block_delta")
        {
            auto delta = obj.value("delta").toObject();
            if (delta.value("type").toString() == "text_delta")
            {
                QString piece = delta.value("text").toString();
                if (!piece.isEmpty() && activeBubble)
                {
                    activeBubble->appendText(piece);
                    scrollToBottom();
                }
            }
        }
        else if (type == "error")
        {
            auto e = obj.value("error").toObject();
            QString msg = e.value("message").toString("Stream error");
            if (activeBubble)
                activeBubble->appendText("\n\n[Ошибка: " + msg + "]");
        }
    }
}

void MainWindow::onStreamFinished()
{
    if (!streamReply)
        return;

    typingTimer->stop();
    typingLabel->hide();

    auto *reply = streamReply;
    streamReply = nullptr;

    QString finalText = activeBubble ? activeBubble->text() : QString();

    if (reply->error() != QNetworkReply::NoError && finalText.isEmpty())
    {
        QString errText = reply->errorString();
        int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QString message = "Ошибка сети: " + errText;
        if (code)
            message = QString("HTTP %1: %2").arg(code).arg(errText);

        QByteArray body = reply->readAll();
        if (!body.isEmpty())
        {
            auto doc = QJsonDocument::fromJson(body);
            if (doc.isObject())
            {
                auto e = doc.object().value("error").toObject();
                QString m = e.value("message").toString();
                if (!m.isEmpty())
                    message += "\n" + m;
            }
        }
        if (activeBubble)
            activeBubble->setText(message);
    }
    reply->deleteLater();

    if (currentChat >= 0 && !finalText.isEmpty())
    {
        chats[currentChat].append({"assistant", finalText});
        saveChats();
    }

    activeBubble = nullptr;
    setStreaming(false);
    sendBtn->setEnabled(true);
    promptEntry->setFocus();
    scrollToBottom();
}

void MainWindow::cancelStream()
{
    if (streamReply)
        streamReply->abort();
}
