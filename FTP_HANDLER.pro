#-------------------------------------------------
#
# Project created by QtCreator 2021-11-19T10:45:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FTP_HANDLER
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

QMAKE_LFLAGS += -no-pie

SOURCES += \       
    main.cpp \
    mainwindow.cpp \
    processbar_widget.cpp

HEADERS += \
    mainwindow.hpp \
    processbar_widget.hpp \
    lib/FtpClient.h \
    lib/Directory.h \
    lib/Log.h \
    lib/FtpClientDirectory.hpp \
    lib/FtpClientFile.hpp \
    lib/FtpClientList.hpp

FORMS += \
    mainwindow.ui \
    processbar_widget.ui

unix:!macx: LIBS += -L$$PWD/lib/ -lftpstack
INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.
unix:!macx: PRE_TARGETDEPS += $$PWD/lib/libftpstack.a

unix:!macx: LIBS += -L$$PWD/lib/ -lsipplatform
INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.
unix:!macx: PRE_TARGETDEPS += $$PWD/lib/libsipplatform.a

RESOURCES += \
    resources/src.qrc
