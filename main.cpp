#include <QApplication>
#include <QCommandLineParser>

#include "appyaiv.h"
#include "yaivmainwindow.h"

int main(int argc, char *argv[])
{
    AppYaiv a(argc, argv);
    QGuiApplication::setApplicationDisplayName(YaivMainWindow::tr("Yet Another Image Viewer"));
    YaivMainWindow yaivMainWindow;
    a.setMainWindow(&yaivMainWindow);
    yaivMainWindow.show();
    return a.exec();
}
