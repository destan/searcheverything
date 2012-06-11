#-------------------------------------------------
#
# Project created by QtCreator 2012-06-10T19:23:45
#
#-------------------------------------------------

QT       += core gui
CONFIG   += console
LIBS     += -lsqlite3

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SearchEverything
TEMPLATE = app

SOURCES += main.cpp\
        SearchWindow.cpp \
    FileSystemIndexer.cpp \
    DatabaseManager.cpp \
    Utils.cpp

HEADERS  += SearchWindow.h \
    FileSystemIndexer.h \
    DatabaseManager.h \
    Utils.h

FORMS    += SearchWindow.ui
