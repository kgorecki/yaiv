VERSION_MAJOR = 0
VERSION_MINOR = 1

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
    "VERSION_MINOR=$$VERSION_MINOR"

VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}

QT += widgets

CONFIG += c++11

TARGET = yaiv
# CONFIG += console
# CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    yaivmainwindow.cpp \
    appyaiv.cpp

HEADERS += \
    yaivmainwindow.h \
    appyaiv.h

mac {
    QMAKE_INFO_PLIST = Info.plist
}
