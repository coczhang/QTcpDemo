QT       += core network

QT       -= gui

TARGET = QSocketClientThread
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    socketclientthread.cpp \
    tcpsocketclient.cpp

HEADERS += \
    socketclientthread.h \
    tcpsocketclient.h
