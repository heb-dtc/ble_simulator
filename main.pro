TEMPLATE = app
TARGET = akolyt

QT += bluetooth
CONFIG += c++11

SOURCES += main.cpp \
    peripheral.cpp \
    messenger.cpp \
    akolytmessenger.cpp

target.path = build/akolyt
INSTALLS += target

HEADERS += \
    peripheral.h \
    messenger.h \
    akolytmessenger.h
