#-------------------------------------------------
#
# Project created by QtCreator 2014-08-05T04:38:43
#
#-------------------------------------------------

QT       += core gui
QT += network

TRANSLATIONS = ServiceUtilities_ja_JP.ts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ServiceUtilities
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    servicetablemodel.cpp \
    consolewidget.cpp \
    servicesettingsdialog.cpp \
    error.cpp

HEADERS  += mainwindow.h \
    servicetablemodel.h \
    consolewidget.h \
    servicesettingsdialog.h \
    error.h

FORMS    += mainwindow.ui \
    consolewidget.ui \
    servicesettingsdialog.ui

RESOURCES += \
    ServiceUtilities.qrc

OTHER_FILES += \
    ServiceUtilities_ja_JP.ts
