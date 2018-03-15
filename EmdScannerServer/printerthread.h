#ifndef PRINTERTHREAD_H
#define PRINTERTHREAD_H

#include <windowsx.h>
//#include <tchar.h>
#include <QThread>
#include <QtNetwork>
#include <QTcpSocket>


class PrinterThread : public QThread
{
public:
    PrinterThread(QObject* parent, qintptr socket);
    virtual void run();
    QString getPrlist();
    QString getAuth();
    void setDefPrinter(QTcpSocket *psocket);
    void doPrint(QTcpSocket *psocket);
    void sndMsg(qint16 cmd,const char vry,QTcpSocket *psocket);

private:
    qintptr _socket;
};


#endif // PRINTERTHREAD_H
