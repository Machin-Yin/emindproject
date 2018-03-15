#-------------------------------------------------
#
# Project created by QtCreator 2016-11-02T14:24:01
#
#-------------------------------------------------

QT       += core gui
QT += printsupport
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ECP
TEMPLATE = app
TRANSLATIONS = lang_zh.ts
RC_FILE += myapp.rc
#QMAKE_LFLAGS += /MANIFESTUAC:"level='requireAdministrator'uiAccess='false'"

SOURCES += main.cpp\
    printerdialog.cpp \
    printerserver.cpp \
    printerthread.cpp \
    package.cpp \
    packagecreate.cpp

HEADERS  += printerdialog.h \
    printerserver.h \
    printerthread.h \
    package.h \
    packagecreate.h

FORMS    += printerdialog.ui


win32: LIBS += -lwinspool -lWs2_32 -liphlpapi
#win32: LIBS += -lIphlpap
RESOURCES += \
    resource.qrc

