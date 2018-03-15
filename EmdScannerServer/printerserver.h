#ifndef PRINTERSERVER_H
#define PRINTERSERVER_H

#include <QTcpServer>
#include <qstring.h>

class PrinterServer : public QTcpServer
{
  public:
        PrinterServer(QObject *parent = 0) : QTcpServer(parent) {}
  protected:
        void incomingConnection(qintptr socketDescriptor);
};


#endif // PRINTERTHREAD_H

