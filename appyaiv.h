#ifndef APPYAIV_H
#define APPYAIV_H

#include <QApplication>
#include <QFileOpenEvent>
#include <QMessageBox>

#include "yaivmainwindow.h"

class AppYaiv : public QApplication
{
//    Q_OBJECT

public:
    AppYaiv(int argc, char *argv[]);
    void setMainWindow(YaivMainWindow *yaivMainWindow);

//protected:
#if defined(Q_OS_OSX)
    bool event(QEvent *);
#endif

private:
    YaivMainWindow *yaivMainWindow;
};

#endif // APPYAIV_H
