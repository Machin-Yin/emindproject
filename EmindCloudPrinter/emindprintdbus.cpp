#include "emindprintdbus.h"
#include <QtDBus/QDBusConnection>
#include <QDebug>
#include <QProcess>
#include "client.h"
namespace EPT {


EmindPrintDbus::EmindPrintDbus(const QString &serviceName, const QString &dbusPath) : QObject()
{
//    eclient = Client::instance();
//    connect(eclient,SIGNAL(setPrinter(QString)),this,SLOT(dbusSndMsg(QString)));

#if 1
    if(QDBusConnection::sessionBus().registerService(serviceName)){
        qDebug()<<__FUNCTION__<<QString("%1 dbus service registered success").arg(serviceName)<<endl;
    }else{
        qDebug()<<__FUNCTION__<<QString("%1 dbus service registered failed").arg(serviceName)<<endl;
    }

    if(QDBusConnection::sessionBus().registerObject(dbusPath,this,QDBusConnection::ExportAllSlots|QDBusConnection::ExportAllSignals)){
        qDebug()<<__FUNCTION__<<"dbus registerObject sessionBus success"<<endl;
    }else{
        qDebug()<<__FUNCTION__<<"dbus registerObject sessionBus failed"<<endl;
    }
#endif

}

EmindPrintDbus::~EmindPrintDbus()
{

}

void EmindPrintDbus::add(const QString& file,const QString& title,const quint32 count,const QString& devUri,const QString& options,bool autoRemove)
{
    qDebug()<<__FUNCTION__<<endl;
    qDebug()<<"count="<<count<<endl;
    qDebug()<<"devUri="<<devUri<<endl;
//    emit getFile(file,title,count,devUri,options,autoRemove);
    client->load(file,title,count,devUri,options,autoRemove);
    qDebug()<<"loaded cupsfile"<<endl;
}

//void EmindPrintDbus::regSessionBus(const QString &serviceName, const QString &dbusPath)
//{
//    if(QDBusConnection::sessionBus().registerObject(dbusPath,this,QDBusConnection::ExportAllSlots)){
//        qDebug()<<"dbus registerObject sessionBus success"<<endl;
//    }else{
//        qDebug()<<"dbus registerObject sessionBus failed"<<endl;
//    }
//    if(QDBusConnection::sessionBus().registerService(serviceName)){
//        qDebug()<<"dbus registerService  sessionBus success"<<endl;
//    }else{
//        qDebug()<<"dbus registerService sessionBus failed"<<endl;
//    }
//}

//void EmindPrintDbus::dbusSndMsg(const QString &msg)
//{
//    qDebug()<<__FUNCTION__<<endl;

//    QDBusMessage msg;
//    QDBusConnection::systemBus.connect("org.emindhelper","/emindhelper","org.emindhelper","setDefaultPrinter",this,);

//    QProcess proc;
//    proc.start(QString("dbus-send --system --type=method_call --print-reply --dest=org.emindhelper /emindhelper org.emindhelper.setDefaultPrinter string:printer"));
//    proc.waitForFinished();


//}


}
