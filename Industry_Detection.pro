#-------------------------------------------------
#
# Project created by QtCreator 2025-03-19T11:31:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Industry_Detection
TEMPLATE = app

CONFIG += c++11
SOURCES += main.cpp\
        mainwindow.cpp \
    cameral.cpp \
    cameralmenu.cpp \
    public.cpp \
    login.cpp \
    role.cpp

HEADERS  += mainwindow.h \
    cameral.h \
    cameralmenu.h \
    public.h \
    login.h \
    role.h

FORMS    += mainwindow.ui
