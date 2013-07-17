QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cyclop-editor
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11


SOURCES += main.cpp

HEADERS += mainwindow.h
SOURCES += mainwindow.cpp
FORMS   += mainwindow.ui

HEADERS += huge_file_qmodel.h
SOURCES += huge_file_qmodel.cpp
