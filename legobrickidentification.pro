#-------------------------------------------------
#
# Project created by QtCreator 2014-01-09T23:10:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = legobrickidentification
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RESOURCES += \
    rsrcfl.qrc

CONFIG   += link_pkgconfig

PKGCONFIG += opencv
