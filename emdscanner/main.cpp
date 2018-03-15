#include <QApplication>
#include <QQmlApplicationEngine>
#include <QTranslator>
#include <QIcon>
#define TRANSLATIONS_DIR "/usr/share/emindscanner/translations/"
#include <QDBusConnection>
#include "clienter.h"
#include <QQmlEngine>
#include <QtQml>
#include <QProcess>
#include <QThread>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qmlRegisterType<Clienter>("com.client.emindprint",1,0,"Clienter");

    //translations
    QTranslator trans;
    if(trans.load(TRANSLATIONS_DIR"zh_CN_1.qm"))
        app.installTranslator(&trans);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    QObject::connect(&engine,SIGNAL(quit()),qApp,SLOT(quit()));
    qApp->setWindowIcon(QIcon(":/img/scanner.png"));

    return app.exec();
}
