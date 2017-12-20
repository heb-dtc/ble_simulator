TEMPLATE = app
TARGET = akolyt

QT += bluetooth
CONFIG += c++11

SOURCES += main.cpp \
    akolyt.cpp

target.path = build/akolyt
INSTALLS += target

HEADERS += \
    akolyt.h
