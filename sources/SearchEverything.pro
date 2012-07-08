#-------------------------------------------------
#
# Project created by QtCreator 2012-06-10T19:23:45
#
#-------------------------------------------------

QT       += core gui
CONFIG   += console
LIBS     += -lsqlite3

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = searcheverything
TEMPLATE = app

SOURCES += main.cpp\
    Utils.cpp \
    SettingsManager.cpp \
    TrayManager.cpp \
    gui/SearchWindow.cpp \
    gui/SettingsWindow.cpp \
    core/FileSystemIndexer.cpp \
    core/DatabaseManager.cpp \
    core/InotifyManager.cpp

HEADERS  += Utils.h \
    SettingsManager.h \
    TrayManager.h \
    gui/SearchWindow.h \
    gui/SettingsWindow.h \
    core/DatabaseManager.h \
    core/FileSystemIndexer.h \
    core/InotifyManager.h

FORMS    += gui/SearchWindow.ui \
    gui/SettingsWindow.ui

RESOURCES += \
    resources/icons/icons.qrc

#Installation
unix:configfiles.extra = make clean
binfile.files += searcheverything
binfile.path = /usr/bin/
iconfiles.files += data/usr/share/searcheverything/*
iconfiles.path = /usr/share/searcheverything
docfiles.files += data/doc/*
docfiles.path = /usr/share/doc/
manfiles.files += data/man/*
manfiles.path = /usr/share/man/man1/
shortcutfiles.files += data/SearchEverything.desktop
shortcutfiles.path = /usr/share/applications/
INSTALLS += iconfiles
INSTALLS += docfiles
INSTALLS += manfiles
INSTALLS += shortcutfiles
INSTALLS += binfile
