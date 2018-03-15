#ifndef CLIENTASSISTANT_H
#define CLIENTASSISTANT_H

#include "client.h"
#include <QObject>

#include "clienter_adaptor.h"  //dbus server side
class ClientAssistant : public QObject
{
    Q_OBJECT

public:
    explicit ClientAssistant(QObject* parent=0);
    ~ClientAssistant();

    Q_INVOKABLE void checkConnectivity(QString ip,QString license);

    Q_PROPERTY(QString pnameStr READ pnameStr WRITE setPnameStr NOTIFY pnameStrChanged)
    QString pnameStr();
    void setPnameStr(const QString);

    Q_PROPERTY(QString serverip READ serverip WRITE setServerip NOTIFY serveripChanged)
    QString serverip();
    void setServerip(const QString ip);
    Q_PROPERTY(QString license READ license WRITE setLicense NOTIFY licenseChanged)
    QString license();
    void setLicense(const QString lic);


    Q_INVOKABLE void setDefaultPrinter(QString prName,quint32 pIndex);

//    Q_PROPERTY(quint8 winVisible READ winVisible WRITE setWinVisible NOTIFY winVisibleChanged)
//    quint8 winVisible();
//    void setWinVisible(const quint8);
    void showWin();

signals:
    void msgClient(QString ,QString );
    void sigConnected();
    void sigAuthWrong();
    void sigConnectRefused();
    void plistSent();
    void stopIndicator();
    void printerSetFinished();
    void pnameStrChanged();
    void sigSetDefaultPrnter(QString,quint32);
    void sigDefaultPrinterDone();
    void sigDefaultPrinterErr();
    void sigPrintSuccess();
    void sigShowWin();
    void sigRunningWarn();
    void serveripChanged();
    void licenseChanged();
    void printerAdded();
    void sigOffline();
    void sigPrnDeleted();
    void sigPrintFailed();

public slots:
    void pnameStrRcved(QString);
    void rcvIp(QString ip,QString lic);
    void onPrinterAdded(QString);
    void onPrnDeleted(QString ppdName);
    void onConnServer(QString,QString);


private:

    QString m_pnameStr;
    quint8 m_visible;
    QString m_serverip;
    QString m_license;

};

#endif // CLIENTASSISTANT_H
