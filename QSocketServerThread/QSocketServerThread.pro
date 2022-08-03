QT       += core network

QT       -= gui

TARGET = QSocketServerThread
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    socketserverthread.cpp \
    tcpsocketserver.cpp

HEADERS += \
    socketserverthread.h \
    tcpsocketserver.h
