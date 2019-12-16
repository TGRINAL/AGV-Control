QT       += core gui widgets network serialport sql multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = P-JAR-01-1_v1.1
TEMPLATE = app
CONFIG += serialport

TRANSLATIONS = agv_delivery_en.ts \
               agv_delivery_zh.ts

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        mywidget.cpp \
    dbwidgt.cpp \
    editdbdialog.cpp \
    failedreasondialog.cpp \
    joystickdialog.cpp \
    warringdialog.cpp \
    listdialog.cpp \
    systemsettingdialog.cpp \
    performancedialog.cpp \
    changepwddialog.cpp \
    batteryitem.cpp

HEADERS += \
        mywidget.h \
    initdb.h \
    dbwidgt.h \
    editdbdialog.h \
    failedreasondialog.h \
    joystickdialog.h \
    warringdialog.h \
    listdialog.h \
    systemsettingdialog.h \
    changepwddialog.h \
    performancedialog.h \
    batteryitem.h

FORMS += \
        mywidget.ui \
    dbwidgt.ui \
    editdbdialog.ui \
    failedreasondialog.ui \
    joystickdialog.ui \
    warringdialog.ui \
    listdialog.ui \
    systemsettingdialog.ui \
    performancedialog.ui \
    changepwddialog.ui \
    batteryitem.ui

RESOURCES += \
    resource.qrc

DISTFILES += \
    Img/volume.ico \
    Img/teco.ico \
    Img/teco.rc

RC_FILE += \
    Img/teco.rc
