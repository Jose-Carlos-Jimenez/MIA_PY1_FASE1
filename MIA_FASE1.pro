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
    FDISK.h \
    MKDISK.h \
    MOUNT.h \
    REP.h \
    RMDISK.h \
    UNMOUNT.h \
    parser.h \
    scanner.h \
    structs.h
