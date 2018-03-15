#ifndef EMINDPRINTDBUS_H
#define EMINDPRINTDBUS_H

#include <QObject>
#include <QString>
#include "client.h"

namespace EPT {


class EmindPrintDbus : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface","org.emindprinter")
public:
    EmindPrintDbus(const QString &serviceName, const QString &dbusPath);
//    explicit EmindPrintDbus();
    ~EmindPrintDbus();

//    void regSessionBus(const QString &serviceName, const QString &dbusPath);


signals:
//    void setDefaultPrinter(QString prName);
    void getFile(const QString& file,const QString& title,const quint32 count,const QString& devUri,const QString& options,bool autoRemove);


public slots:
    void add(const QString& file,const QString& title,const quint32 count,const QString& devUri,const QString& options = "",bool autoRemove = false);

//    void dbusSndMsg(const QString &msg);

private:
    Client* eclient;
};

}
#endif // EMINDPRINTDBUS_H
