#ifndef CLIENTER_H
#define CLIENTER_H

#include <QObject>
#include <QThread>
#include "clienter_interface.h"
//dbus client side
class Clienter : public QObject
{
    Q_OBJECT
public:
    explicit Clienter();
    ~Clienter();

    Q_PROPERTY(QString pnameStr READ pnameStr WRITE setPnameStr NOTIFY pnameStrChanged)
    QString pnameStr();
    void setPnameStr(const QString);

    Q_PROPERTY(QString pnameAdded READ pnameAdded WRITE setPnameAdded NOTIFY pnameAddedChanged)
    QString pnameAdded();
    void setPnameAdded(const QString);

    Q_PROPERTY(QString pnameDel READ pnameDel WRITE setPnameDel NOTIFY pnameDelChanged)
    QString pnameDel();
    void setPnameDel(const QString);

    Q_PROPERTY(QString servip READ servip WRITE setServip NOTIFY servipChanged)
    QString servip();
    void setServip(const QString);

signals:
    Q_INVOKABLE void sigConnServer(QString,QString);
    Q_INVOKABLE void sigSetDefPrinter(QString,QString,qint32);
    Q_INVOKABLE void searchPPD();

    void sigConnected();
    void sigAuthWrong();
    void sigConnectRefused();
    void sigPlistSent();
    void pnameStrChanged();
    void printerSetFinished();
    void sigSetDefaultPrnter(QString,QString,quint32);
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
    void pnameAddedChanged();
    void pnameDelChanged();
    void servipChanged();


public slots:
    void onSigConnected(QString);
    void onSigAuthWrong(QString);
    void onSigConnectRefused(QString);
    void onPnameStrRcved(QString,QString);
    void onPrinterSetFinished();
    void onSigDefaultPrinterDone();
    void onSigDefaultPrinterErr();
    void onSigPrintSuccess();
    void onSigShowWin();
    void onSigRunningWarn();
    void onServeripChanged();
    void onLicenseChanged();
    void onPrinterAdded(QString);
    void onSigOffline();
    void onSigPrnDeleted(QString);
    void onSigPrintFailed();



private:
    QString m_pnameStr;
    QString m_pnameAdded;
    QString m_pnameDel;
    QString m_sip;
    org::emindprinter::ClienterInterface *m_clientInf;

};

#endif // CLIENTER_H
