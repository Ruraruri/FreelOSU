#-------------------------------------------------
#
# Project created by QtCreator 2019-01-11T20:44:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FreelOSU
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        freelosu.cpp \
    main.cpp \
    memory.cpp \
    osu.cpp \
    player.cpp \
    settings.cpp

HEADERS += \
        freelosu.h \
    input.h \
    memory.h \
    osu.h \
    player.h \
    settings.h \
    main.h \
    global.h

FORMS += \
        freelosu.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    freelosu.qrc


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Qt/Qt5.12.0/5.12.0/msvc2017/lib/ -lqtmain
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Qt/Qt5.12.0/5.12.0/msvc2017/lib/ -lqtmaind
else:unix: LIBS += -L$$PWD/../../Qt/Qt5.12.0/5.12.0/msvc2017/lib/ -lqtmain

INCLUDEPATH += $$PWD/../../Qt/Qt5.12.0/5.12.0/msvc2017/include
DEPENDPATH += $$PWD/../../Qt/Qt5.12.0/5.12.0/msvc2017/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Qt/Qt5.12.0/5.12.0/msvc2017/lib/libqtmain.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Qt/Qt5.12.0/5.12.0/msvc2017/lib/libqtmaind.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Qt/Qt5.12.0/5.12.0/msvc2017/lib/qtmain.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Qt/Qt5.12.0/5.12.0/msvc2017/lib/qtmaind.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../Qt/Qt5.12.0/5.12.0/msvc2017/lib/libqtmain.a

RC_ICONS = FreelOSU_ico.ico
