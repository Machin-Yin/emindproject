#include "printerserver.h"
#include "printerthread.h"
#include <qprinterinfo.h>


void PrinterServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug()<<__FUNCTION__<<endl;
    PrinterThread* worker = new PrinterThread(this, socketDescriptor);
    connect(worker,SIGNAL(finished()),worker,SLOT(deleteLater()));
    worker->start();
}



