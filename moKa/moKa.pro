#-------------------------------------------------
#
# Project created by QtCreator 2017-05-12T23:49:46
#
#-------------------------------------------------

QT       += core gui
LIBS     += -lportaudio -lrtmidi

QMAKE_CXXFLAGS += -O3 -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = moKa
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    patch.cpp \
    envelope.cpp \
    waveform.cpp \
    modulation.cpp \
    RtMIDIInterface.cpp \
    AudioInterface.cpp \
    managerconsole.cpp \
    filter.cpp \
    effect.cpp

HEADERS  += mainwindow.h \
    patch.h \
    envelope.h \
    waveform.h \
    modulation.h \
    RtMIDIInterface.h \
    AudioInterface.h \
    managerconsole.h \
    filter.h \
    effect.h

DISTFILES += \
    README.md
