#include <QApplication>
#include <QCommandLineParser>

#include "yaivmainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QGuiApplication::setApplicationDisplayName(YaivMainWindow::tr("Yet Another Image Viewer"));
    YaivMainWindow yaivMainWindow;
    yaivMainWindow.show();
    return a.exec();
}
