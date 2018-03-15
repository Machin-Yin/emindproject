######################################################################
# Automatically generated by qmake (3.0) ?? 3? 2 17:56:02 2017
######################################################################

TEMPLATE = app
TARGET = emindprinter-daemon
QT += qml quick network widgets printsupport dbus core
CONFIG += c++11
DBUS_ADAPTORS += clienter.xml

# Input
HEADERS += client.h \
           clientassistant.h \
           clientsock.h \
           clientsystemtray.h \
           cupsbackend.h \
           emindprintdbus.h \
           logger.h \
           mainapplication.h \
           printerlistmodel.h \
           printerwatcher.h \
           protocol.h \
           qmlpropertyop.h \
           servermanager.h \
           tcpthread.h \
           qtlockedfile/qtlockedfile.h \
           qtsingleapplication/qtlocalpeer.h \
           qtsingleapplication/qtsingleapplication.h \
           qtsingleapplication/qtsinglecoreapplication.h \
    package.h \
    packagecreate.h \
    debian/postinst \
    debian/rules
SOURCES += client.cpp \
           clientassistant.cpp \
           clientsock.cpp \
           clientsystemtray.cpp \
           cupsbackend.cpp \
           emindprintdbus.cpp \
           logger.cpp \
           main.cpp \
           mainapplication.cpp \
           printerlistmodel.cpp \
           printerwatcher.cpp \
           qmlpropertyop.cpp \
           servermanager.cpp \
           tcpthread.cpp \
           qtlockedfile/qtlockedfile.cpp \
           qtlockedfile/qtlockedfile_unix.cpp \
           qtsingleapplication/qtlocalpeer.cpp \
           qtsingleapplication/qtsingleapplication.cpp \
           qtsingleapplication/qtsinglecoreapplication.cpp \
    package.cpp \
    packagecreate.cpp
RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target


DISTFILES += \
    clienter.xml \
    cups/emindprinter \
    cups/emindprinterbackend \
    debian/postrm \
    cups/emindprinter.ppd \
    debian/source/format \
    debian/compat \
    debian/control \
    debian/copyright \
    debian/changelog \
    debian/install \
    misc/emindprinter_daemon.desktop
