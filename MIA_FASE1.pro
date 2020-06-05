TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle

SOURCES += \
        main.cpp \
        parser.cpp \
        scanner.cpp

DISTFILES += \
    lexico.l \
    sintactico.y

HEADERS += \
    MKDISK.h \
    parser.h \
    scanner.h
