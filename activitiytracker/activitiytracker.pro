QT += core network
QT -= gui

CONFIG += c++14

TARGET = activitiytracker
CONFIG += console
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -std=c++1z
TEMPLATE = app
INCLUDEPATH += "qhttp/src"
SOURCES += main.cpp \
    postgres.cpp \
    postgres_prepared_statements.cpp \
    activity_event.cpp \
    rest.cpp \
    db_helpers.cpp \
    request_handling.cpp \
    excel_export.cpp
LIBS += -lqhttp -lpqxx -lpq -lxlnt
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    postgres.h \
    activity_event.h \
    globals.h \
    rest.h \
    rest_helpers.h \
    db_helpers.h \
    request_handling.h \
    responses.h \
    excel_export.h
