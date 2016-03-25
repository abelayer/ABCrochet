#-------------------------------------------------
#
# Project created by QtCreator 2016-03-03T12:34:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
qtHaveModule(printsupport): QT += printsupport

TARGET = ABCrochet
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    GridWidget.cpp \
    NewGridDialog.cpp \
    ConvertToBWDialo.cpp \
    UndoCommand.cpp

HEADERS  += mainwindow.h \
    GridWidget.h \
    NewGridDialog.h \
    ConvertToBWDialog.h \
    UndoCommand.h

FORMS    += mainwindow.ui \
    NewGridDialog.ui \
    ConvertToBWDialog.ui

RESOURCES += icons/data.qrc

RC_ICONS += icons/Om.ico

VERSION = 1.1.0.0
QMAKE_TARGET_COMPANY = Om Shanti
QMAKE_TARGET_PRODUCT = "AB Crochet"
QMAKE_TARGET_COPYRIGHT = Copyright (c) Andy Bray
