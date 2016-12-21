#include "appyaiv.h"

AppYaiv::AppYaiv(int argc, char *argv[])
    : QApplication(argc, argv)
{
}

void AppYaiv::setMainWindow(YaivMainWindow *yaivMainWindow)
{
    this->yaivMainWindow = yaivMainWindow;
}

// *** protected
#if defined(Q_OS_OSX)
bool AppYaiv::event(QEvent *event)
{
/*    switch (event->type())
    {
        case QEvent::FileOpen:
            yaivMainWindow->openFile(static_cast<QFileOpenEvent *>(event)->file());
            return true;
        default:
            break;
    }*/
    return QApplication::event(event);
}
#endif
