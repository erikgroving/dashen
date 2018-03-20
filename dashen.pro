TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    src/searchclient/strategy.cpp \
    src/searchclient/heuristic.cpp \
    src/searchclient/searchclient.cpp \
    src/searchclient/state.cpp \
    src/searchclient/command.cpp \
    src/main.cpp

HEADERS += \
    src/searchclient/strategy.h \
    src/searchclient/heuristic.h \
    src/searchclient/searchclient.h\
    src/searchclient/state.h \
    src/searchclient/command.h
