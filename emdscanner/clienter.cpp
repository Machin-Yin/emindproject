#include "clienter.h"
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDebug>
#include <QObject>
#define PPD_LOCATION  "/etc/cups/ppd/"


Clienter::Clienter()
{
    m_clientInf = new org::emindprinter::ClienterInterface("org.emindprinter.emgui","/Clienter",QDBusConnection::sessionBus(),this);

    connect(this,SIGNAL(sigConnServer(QString,QString)),m_clientInf,SLOT(onConnServer(QString,QString)));
    connect(m_clientInf,SIGNAL(sigConnected(QString)),this,SLOT(onSigConnected(QString)));
    connect(m_clientInf,SIGNAL(sigAuthWrong(QString)),this,SLOT(onSigAuthWrong(QString)));
    connect(m_clientInf,SIGNAL(sigConnectRefused(QString)),this,SLOT(onSigConnectRefused(QString)));
    connect(m_clientInf,SIGNAL(pnameStrSent(QString,QString)),this,SLOT(onPnameStrRcved(QString,QString)));
    connect(this,SIGNAL(sigSetDefPrinter(QString,QString,qint32)),m_clientInf,SLOT(setDefaultPrinter(QString,QString,qint32)));
    connect(m_clientInf,SIGNAL(printerAdded(QString)),this,SLOT(onPrinterAdded(QString)));
    connect(this,SIGNAL(searchPPD()),m_clientInf,SLOT(searchPPD()));
    connect(m_clientInf,SIGNAL(sigPrnDeleted(QString)),this,SLOT(onSigPrnDeleted(QString)));




}

Clienter::~Clienter()
{
}

void Clienter::onSigConnected(QString sip)
{
    qDebug()<<__FUNCTION__<<"m_sip="<<m_sip<<endl;
    if(m_sip == sip)
        emit sigConnected();
}

void Clienter::onSigAuthWrong(QString sip)
{
    if(m_sip == sip)
        emit sigAuthWrong();
}

void Clienter::onSigConnectRefused(QString sip)
{
    qDebug()<<__FUNCTION__<<"sip="<<sip<<endl;
    if(m_sip == sip)
        emit sigConnectRefused();
}

void Clienter::onPrinterSetFinished()
{

}

void Clienter::onPnameStrRcved(QString sip,QString pname)
{
    if(m_sip == sip){
        qDebug()<<__FUNCTION__<<"  pname="<<pname<<endl;
        setPnameStr(pname);
        emit sigPlistSent();
    }
}


QString Clienter::pnameStr()
{
    return m_pnameStr;
}

void Clienter::setPnameStr(const QString str)
{
    m_pnameStr = str;
    emit pnameStrChanged();
}

QString Clienter::pnameAdded()
{
    return m_pnameAdded;
}

void Clienter::setPnameAdded(const QString str)
{
    m_pnameAdded = str;
    emit pnameAddedChanged();
}

QString Clienter::pnameDel()
{
    return m_pnameDel;
}

void Clienter::setPnameDel(const QString str)
{
    m_pnameDel = str;
    emit pnameDelChanged();
}


QString Clienter::servip()
{
    qDebug()<<__FUNCTION__<<"m_sip="<<m_sip<<endl;
    return m_sip;
}

void Clienter::setServip(const QString str)
{
    m_sip = str;
    emit servipChanged();
    qDebug()<<__FUNCTION__<<"m_sip="<<m_sip<<endl;

}

void Clienter::onSigDefaultPrinterDone()
{

}

void Clienter::onSigDefaultPrinterErr()
{

}

void Clienter::onSigPrintSuccess()
{
    emit sigPrintSuccess();
}

void Clienter::onSigPrintFailed()
{
    emit sigPrintFailed();
}

void Clienter::onSigShowWin()
{

}

void Clienter::onSigRunningWarn()
{

}

void Clienter::onServeripChanged()
{

}

void Clienter::onLicenseChanged()
{

}

void Clienter::onPrinterAdded(QString pnameAdded)
{
    qDebug()<<__FUNCTION__<<"pnameAdded="<<pnameAdded<<endl;
    if(!pnameAdded.isEmpty()){
        if(m_sip == pnameAdded.split("@").at(1)){
            pnameAdded = pnameAdded.split("@").at(0);
            setPnameAdded(pnameAdded);
            emit printerAdded();
        }
    }
}

void Clienter::onSigOffline()
{

}



void Clienter::onSigPrnDeleted(QString prnDelete)
{
    QString tmpstr = prnDelete.split("/").at(4);
    tmpstr = tmpstr.split("@").at(1);
    tmpstr.chop(4);
    qDebug()<<__FUNCTION__<<"tmpstr="<<tmpstr<<endl;
    if(tmpstr == m_sip)
        if(!QFile(prnDelete).exists()){
            qDebug()<<"printer deleted="<<prnDelete<<endl;
            QString tmps = prnDelete.split("@").at(0);
            tmps = tmps.split("/").at(4);
            qDebug()<<__FUNCTION__<<"tmps="<<tmps<<endl;
            setPnameDel(tmps);
            emit sigPrnDeleted();
        }
}


