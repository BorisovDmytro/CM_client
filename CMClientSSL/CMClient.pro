#-------------------------------------------------
#
# Project created by QtCreator 2017-05-08T17:25:32
#
#-------------------------------------------------

QT       += core gui multimedia websockets
CONFIG   += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CMClient
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    messageinformation.cpp \
    cmclientengen.cpp \
    account.cpp \
    threadrecord.cpp \
    gui.cpp \
    dialogcall.cpp

HEADERS  += mainwindow.h \
    messageinformation.h \
    cmclientengen.h \
    account.h \
    threadrecord.h \
    gui.h \
    dialogcall.h

FORMS    += mainwindow.ui \
    gui.ui \
    dialogcall.ui

RESOURCES += \
    res.qrc
