#-------------------------------------------------
#
# Project created by QtCreator 2019-07-13T19:09:47
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CloudDisk
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
        common/downloadlayout.cpp \
        common/downloadtask.cpp \
        common/downloadthread.cpp \
        common/uploadlayout.cpp \
        common/uploadtasks.cpp \
        common/uploadthread.cpp \
        common/utils.cpp \
        common/logininfoinst.cpp \
        login.cpp \
        main.cpp \
        mainwindow.cpp \
        myfile.cpp \
        toolswt/buttongroup.cpp \
        toolswt/dataprogress.cpp \
        toolswt/fileproperty.cpp \
        toolswt/titlewidget.cpp \
        transfer.cpp

HEADERS += \
        common/downloadlayout.h \
        common/downloadtask.h \
        common/downloadthread.h \
        common/uploadlayout.h \
        common/uploadtasks.h \
        common/uploadthread.h \
        common/utils.h \
        common/logininfoinst.h \
        login.h \
        mainwindow.h \
        myfile.h \
        toolswt/buttongroup.h \
        toolswt/dataprogress.h \
        toolswt/fileproperty.h \
        toolswt/titlewidget.h \
        transfer.h

FORMS += \
        login.ui \
        mainwindow.ui \
        myfile.ui \
        toolswt/buttongroup.ui \
        toolswt/dataprogress.ui \
        toolswt/fileproperty.ui \
        toolswt/titlewidget.ui \
        transfer.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

LIBS += $$quote("D:/Qt/Tools/OpenSSL/Win_x86/lib/libssl.lib")
LIBS += $$quote("D:/Qt/Tools/OpenSSL/Win_x86/lib/libcrypto.lib")

INCLUDEPATH += $$quote("D:/Qt/Tools/OpenSSL/src/include")
