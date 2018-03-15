#include "mainapplication.h"
#include <QDebug>
#include <QDir>
#include "qmlpropertyop.h"
#include "client.h"
#include "clientassistant.h"

namespace EPT {

MainApplication::MainApplication(int &argc,char** argv):SharedTools::QtSingleApplication("emindprinter",argc,argv),
    _logExpire(0),
    _logFlush(false)
{
    if(isRunning()){
        qDebug()<<"emindprinter-daemon is running"<<endl;
    }
//    setQuitOnLastWindowClosed(false);
    _logDir = QString(QDir::homePath()+"/.cache/");


//    QThread logthread;
//    Logger *logger = new Logger;
//    logger->moveToThread(&logthread);
//    connect(&logthread,&QThread::finished,logger,&QObject::deleteLater);
//    logthread.start();

//    setupLogging();

//        QThread clientthread;
//        Logger *logger = new Logger;
//        logger->moveToThread(&logthread);
//        connect(&logthread,&QThread::finished,logger,&QObject::deleteLater);
//        logthread.start();
}

MainApplication::~MainApplication()
{

}

void MainApplication::setupLogging()
{
    // might be called from second instance
    Logger::instance()->setLogFile(_logFile);
    Logger::instance()->setLogDir(_logDir);
    Logger::instance()->setLogExpire(_logExpire);
    Logger::instance()->setLogFlush(_logFlush);
    Logger::instance()->enterNextLogFile();

//    qDebug() << QString::fromLatin1( "################## %1 %2 (%3) %4").arg(_theme->appName())
//                .arg( QLocale::system().name() )
//                .arg(property("ui_lang").toString())
//                .arg(_theme->version());

}


}
