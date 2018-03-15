#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include "client.h"
#include <QMetaObject>
#include <QVariant>
#include <QString>
#include "cupsbackend.h"
#include <QQuickView>
#include <QQmlContext>
#include <QtQuick/QQuickItem>
#include "emindprintdbus.h"
#include "printerlistmodel.h"
#include <QTranslator>
#include "clientassistant.h"
#include <mainapplication.h>
#include "qmlpropertyop.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include "clientsystemtray.h"
#include "protocol.h"
#include "clientassistant.h"

using namespace EPT;

//QGuiApplication *app111=NULL;

void readEnvFile()
{
    QFile envFile(QDir::homePath() + "/.cache/emindprinter.env");
    if(envFile.exists()){
        envFile.open(QFile::ReadOnly);
        while(!envFile.atEnd()){
            QString line = QString::fromLocal8Bit(envFile.readLine());
            QString name = line.section("=",0,0).trimmed();
            QString value = line.section("=",1).trimmed();
            if((value.startsWith('\'')&&value.endsWith('\'') )||( (value.startsWith('"')&&value.startsWith('"')))){
                value = value.mid(1,value.length()-2);
            }

            qputenv(name.toLocal8Bit(),value.toLocal8Bit());
        }
        envFile.close();
    }
}

int main(int argc, char *argv[])
{
    readEnvFile();
    //    QGuiApplication app(argc, argv);
    EPT::MainApplication app(argc,argv);
    //    app111 = &app;

    if(app.isRunning() ){
//        QMessageBox::warning(0,QObject::tr("emindprinter"),QObject::tr("emindprinter is running"));
        return 0;
    }
    //    qInstallMessageHandler(logger);
    //    qApp->setOrganizationName(ORG_NAME);
#if 0
    if(QSystemTrayIcon::isSystemTrayAvailable()){
        qDebug()<<"SystemTray is available"<<endl;
    }else{
        qDebug()<<"SystemTray is not available"<<endl;
    }

    //client register
    qmlRegisterType<ClientAssistant>("com.client.emindprint",1,0,"ClientAssistant");

    qmlRegisterType<Client>("com.client.emindprint",1,0,"EmindClient");
    //    qmlRegisterType<CupsBackend>("com.client.emindprint",1,0,"CupsBackend");
    //    qmlRegisterType<Printer>("com.client.emindprint",1,0,"Printer");
    //    qmlRegisterType<PrinterListModel>("com.client.emindprint",1,0,"PrinterListModel");


    //translations
    QTranslator trans;
    if(trans.load(TRANSLATIONS_DIR"zh_CN.qm"))
        app.installTranslator(&trans);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    //    engine.load(QUrl(QStringLiteral("qrc:/PrinterList.qml")));
    //    engine.load(QUrl(QStringLiteral("qrc:/ClientLogin.qml")));

    QObject::connect(&engine,SIGNAL(quit()),qApp,SLOT(quit()));

    //    QObject* root1 = NULL;
    QObject* root2 = NULL;
    QList<QObject*> rootObjs = engine.rootObjects();
    qDebug()<<"rootObjs.length()="<<rootObjs.length()<<endl;

    for(int i=0;i<rootObjs.size();i++){
        //        if(rootObjs.at(i)->objectName() == "PrinterWin"){
        //            root1 = rootObjs.at(i);
        //        }
        if(rootObjs.at(i)->objectName() == "rootObj"){
            root2 = rootObjs.at(i);
            //            qDebug()<<"root2 found"<<endl;
        }
        break;
    }

    new EPT::ClientSystemTray(root2);

    qApp->setWindowIcon(QIcon(":/img/print32.png"));
#endif


//    if(QFile(CONFFile).exists())
//        client->restore();

    //to handle cups files
    QStringList files;
    QStringList titles;
    QStringList args = app.arguments();
    bool autoRemove = false;

    for(int i=1;i < args.count();++i){
        QString arg = args.at(i);
        if(arg == "-t" || arg == "--title"){
            if(i+1 < args.count()){
                titles<<args.at(i+1);
                i++;
                continue;
            }else{
                return 1;
            }
        }

        files << args.at(i);
    }

    EmindPrintDbus emDbus("org.emindprinter","/emindprinter");
    //    EmindPrintDbus em("org.emindhelper","/emindhelper");

    //    EmindPrintDbus emBackend;
    //    emBackend.regSessionBus("org.emindprinter","/emindprinter");
    qApp->setQuitOnLastWindowClosed(false);

    app.processEvents();

//    client->load(files,titles,0,"","",autoRemove);
    client->loadCupsFiles(files,titles,0,"","",autoRemove);

    return app.exec();
}


