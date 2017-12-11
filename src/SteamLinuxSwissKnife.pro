#===================================================================================
# ---[SLSK - Qt QMake Profile / Project File]---
# Copyright (C) 2017 Supremist (aka supremesonicbrazil)
# This file is part of Steam Linux Swiss Knife (or SLSK for short).
# Steam Linux Swiss Knife is available under the GNU GPL v3.0 license. See the
# accompanying COPYING file for more details.
#===================================================================================

#-------------------------------------------------
#
# Project created by QtCreator 2017-10-30T11:03:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SteamLinuxSwissKnife
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mainmenu.cpp \
    db.cpp \
    sqlite3.c \
    progressmenu.cpp

HEADERS  += mainwindow.h \
    mainmenu.h \
    db.h \
    sqlite3.h \
    progressmenu.h

FORMS    += mainwindow.ui

CONFIG += c++11

LIBS += -ldl
