#include "mainwindow.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("Claude");
    QApplication::setOrganizationName("Claude");
    QApplication::setWindowIcon(QIcon("claude.png"));

    MainWindow w;
    w.show();
    return a.exec();
}
