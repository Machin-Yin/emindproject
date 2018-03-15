#include "clientassistant.h"
#include "tcpthread.h"

ClientAssistant::ClientAssistant(QObject* parent) : QObject(parent),m_serverip(""),m_license("")
{    
//    new ClienterInterfaceAdaptor(this);
//    QDBusConnection dbusConn = QDBusConnection::sessionBus();
//    bool ret = dbusConn.registerService("org.emindprinter.emgui");
//    if(ret)
//        qDebug()<<__FUNCTION__<<"register service success"<<endl;
//    else
//        qDebug()<<__FUNCTION__<<"register service failed"<<endl;
//    ret = dbusConn.registerObject("/Clienter",this);
//    if(ret)
//        qDebug()<<__FUNCTION__<<"register object success"<<endl;
//    else
//        qDebug()<<__FUNCTION__<<"register object failed"<<endl;


//    connect(this,SIGNAL(msgClient(QString,QString)),client,SLOT(checkConnectivity(QString,QString)));
//    connect(client,SIGNAL(sigConnected()),m_clientInf,SLOT(onSigConnected()));
//    connect(client,SIGNAL(sigAuthWrong()),m_clientInf,SLOT(onSigAuthWrong()));
//    connect(client,SIGNAL(sigConnectRefused()),m_clientInf,SLOT(onSigConnectRefused()));
//    connect(client,SIGNAL(plistSent()),m_clientInf,SIGNAL(plistSent()));
//    connect(client,SIGNAL(printerSetFinished()),m_clientInf,SIGNAL(printerSetFinished()));
//    connect(client,SIGNAL(pnameStrSent(QString)),m_clientInf,SLOT(onPnameStrRcved(QString)));
//    connect(m_clientInf,SIGNAL(sigSetDefPrinter(QString,qint32)),client,SLOT(setDefaultPrinter(QString,qint32)));
//    connect(client,SIGNAL(sigDefaultPrinterDone()),m_clientInf,SIGNAL(sigDefaultPrinterDone()));
//    connect(client,SIGNAL(sigDefaultPrinterErr()),m_clientInf,SIGNAL(sigDefaultPrinterErr()));
//    connect(client,SIGNAL(stopIndicator()),this,SIGNAL(stopIndicator()));
//    connect(client,SIGNAL(sigPrintSuccess()),m_clientInf,SLOT(onSigPrintSuccess()));
//    connect(client,SIGNAL(sigPrintFailed()),m_clientInf,SLOT(onSigPrintFailed()));

//    connect(client,SIGNAL(sigShowWin()),this,SIGNAL(sigShowWin()));
//    connect(client,SIGNAL(sigShowWin()),this,SLOT(showWin()));
//    connect(client,SIGNAL(sigRunningWarn()),m_clientInf,SIGNAL(sigRunningWarn()));
//    connect(client,SIGNAL(sigPassIp(QString,QString)),this,SLOT(rcvIp(QString,QString)));
//    connect(client,SIGNAL(printerAdded(QString)),this,SLOT(onPrinterAdded(QString)));
//    connect(client,SIGNAL(sigOffline()),m_clientInf,SIGNAL(sigOffline()));
//    connect(client,SIGNAL(sigPrnDeleted(QString)),this,SLOT(onPrnDeleted(QString)));
}

void ClientAssistant::showWin()
{
    emit sigShowWin();
    qDebug()<<__FUNCTION__<<endl;
}

void ClientAssistant::checkConnectivity(QString ip,QString license)
{
     emit msgClient(ip,license);
}

ClientAssistant::~ClientAssistant()
{

}

void ClientAssistant::pnameStrRcved(QString pnamelist)
{
    qDebug()<<__FUNCTION__<<endl;
    setPnameStr(pnamelist);
}

QString ClientAssistant::serverip()
{
    return m_serverip;
}

void ClientAssistant::setServerip(const QString ip)
{
    m_serverip = ip;
    emit serveripChanged();
}

void ClientAssistant::setLicense(QString lic)
{
    m_license = lic;
    emit licenseChanged();
}

QString ClientAssistant::license()
{
    return m_license;
}


QString ClientAssistant::pnameStr()
{
    return m_pnameStr;
}

void ClientAssistant::setPnameStr(const QString str)
{
    m_pnameStr = str;
    emit pnameStrChanged();
}

void ClientAssistant::setDefaultPrinter(QString prName, quint32 pIndex)
{
    emit sigSetDefaultPrnter(prName,pIndex);
}

void ClientAssistant::rcvIp(QString ip, QString lic)
{
    setServerip(ip);
    setLicense(lic);
}

void ClientAssistant::onPrinterAdded(QString pName)
{
    qDebug()<<__FUNCTION__<<endl;
    qDebug()<<"pName="<<pName<<endl;
    setPnameStr(pName);
    emit printerAdded();
}

void ClientAssistant::onPrnDeleted(QString ppdName)
{
    qDebug()<<__FUNCTION__<<endl;
    setPnameStr(ppdName);
    emit sigPrnDeleted();
}

void ClientAssistant::onConnServer(QString serverip,QString license)
{
    qDebug()<<__FUNCTION__<<endl;
//    client->checkConnectivity(serverip,license);
}


