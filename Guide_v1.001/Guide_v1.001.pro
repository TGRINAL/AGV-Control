QT       += core gui widgets network sql multimedia quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Guide_v1.001
TEMPLATE = app

TRANSLATIONS = HTY_en.ts \
               HTY_zh.ts

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        mywidget.cpp \
    failedreasondialog.cpp \
    joystickdialog.cpp \
    warringdialog.cpp \
    listdialog.cpp \
    systemsettingdialog.cpp \
    changepwddialog.cpp \
    batteryitem.cpp \
    resetorigindialog.cpp \
    recorddialog.cpp \
    obstaclesalarmdialog.cpp \
    soundmodifydialog.cpp

HEADERS += \
        mywidget.h \
    initdb.h \
    failedreasondialog.h \
    joystickdialog.h \
    warringdialog.h \
    listdialog.h \
    systemsettingdialog.h \
    changepwddialog.h \
    batteryitem.h \
    resetorigindialog.h \
    recorddialog.h \
    obstaclesalarmdialog.h \
    soundmodifydialog.h

FORMS += \
        mywidget.ui \
    failedreasondialog.ui \
    joystickdialog.ui \
    warringdialog.ui \
    listdialog.ui \
    systemsettingdialog.ui \
    changepwddialog.ui \
    batteryitem.ui \
    resetorigindialog.ui \
    recorddialog.ui \
    obstaclesalarmdialog.ui \
    soundmodifydialog.ui

RESOURCES += \
    resource.qrc

DISTFILES += \
    Img/volume.ico \
    Img/hty.ico \
    Img/hty.rc \
    Img/hty.ico \
    Img/volume.ico \
    Img/hty.rc \
    Img/background.png \
    Img/finger.png

RC_FILE += \
    Img/hty.rc
