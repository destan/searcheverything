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
        SearchWindow.cpp \
    FileSystemIndexer.cpp \
    DatabaseManager.cpp \
    Utils.cpp \
    InotifyManager.cpp \
    SettingsManager.cpp \
    TrayManager.cpp

HEADERS  += SearchWindow.h \
    FileSystemIndexer.h \
    DatabaseManager.h \
    Utils.h \
    InotifyManager.h \
    SettingsManager.h \
    TrayManager.h

FORMS    += SearchWindow.ui

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
