#ifndef CLIENT_H
#define CLIENT_H

#include <QMessageBox>
#include <QObject>
#include <QAbstractSocket>
#include <QStringListModel>
#include "tcpthread.h"
#include <QGuiApplication>
#include <QStandardPaths>
#include "protocol.h"
#include "clienter_adaptor.h"  //dbus server side
#include "clientsock.h"
#include <QMap>
#include <QSharedPointer>

class Package;
class QFile;
class QTcpSocket;
class QFileSystemWatcher;
typedef QSharedPointer<ClientSock> ClientSockPtr;

namespace EPT {

//class PrinterListModel;
//class ClientPrive;
class Client : public QObject
{
    Q_OBJECT
//    Q_PROPERTY(QStringListModel pNameListModel READ getPNameListModel NOTIFY pNameListModelChanged)
//    Q_PROPERTY(QString err READ getErr WRITE setErr NOTIFY errChanged)
//    Q_PROPERTY(QStringList printerNameList READ printerNameList WRITE setPrinterNameList NOTIFY printerNameListChanged)
//    Q_PROPERTY(QString pnameStr READ pnameStr WRITE setPnameStr NOTIFY pnameStrChanged)
    Q_ENUMS(ServerError)


public:
    explicit Client(QObject *parent = 0);
    ~Client();

    static Client* instance();
    enum ServerError{ IpError,LicenseError};

    void sendFiles(const QString fileName,const QString title,const quint32 count,const QString& devUri,const QString& options,bool autoRemove);


    void load(const QString& fileName,const QString& title,const quint32 count,const QString& devUri,const QString& options,bool autoRemove);

    void loadCupsFiles(const QStringList& fileNames,const QStringList& titles,const quint32 count,const QString& devUri,const QString& options,bool autoRemove);

    void restore();

    void watchPrinters();
    void createConnection(QString ip,QString license);
    void connToServer(QString,QString);
    bool checkLicense(QString ip,QString license);
    QString getPrnlist(QString ip,QString license);
    bool sndFile(const QString& fileName,const QString& title,const quint32 count,const QString& devUri,const QString& options,bool autoRemove);
    QString readConfig(QString ip);
    void cancelJob(QString fileToPrint);
    void releaseJob(QString fileToPrint);



signals:
    void error();
    void errChanged();
    void errConnected();
    void printerNameListChanged();
    void pNameListModelChanged(QStringList);
    void rcvCupsFile();
    void sigConnected(QString);
    void sigAuthWrong(QString);
    void sigConnectRefused(QString);
    void pnameStrChanged();
    void plistSent();
    void printerSetFinished();
    void stopIndicator();
    void pnameStrSent(QString,QString);
    void sigDefaultPrinterDone();
    void sigDefaultPrinterErr();
    void setPrinter(QString prName);
    void sigPrintSuccess();
    void sigShowWin(quint8);
    void sigRunningWarn();
    void sigPassIp(QString ip,QString lic);
    void printerAdded(QString);
    void sigOffline();
    void sigPrnDeleted(QString);
    void sigPrintFailed();
    void sigload(const QStringList& fileNames,const QStringList& titles,const quint32 count,const QString& devUri,const QString& options,bool autoRemove);
    void sigDefPrinter(qint32);
    void findPPD();
    void rmfiles();



public slots:
    void setDefaultPrinter(QString serverIP, QString prName, qint32 pIndex);
//    void getPrinterNameList(QString& msg);
//    Q_INVOKABLE void checkConnectivity(QString ip,QString license);
    void onConnServer(QString,QString);
//    void checkLicense();
    void displayError(QAbstractSocket::SocketError);
//    void updateClientProgress(qint64 numBytes);
//    void onReadyRead();
    void resumeConnect();
    void savConnection(QString sip,QString printerlist);
    void savConnection(QString sip);
    void searchPPD();
    void prnDelete(QString);
    void rmFiles();

private:
    bool sendPackage(QTcpSocket* socket, const Package* package, qint64 size);
    Package* readPackage(QTcpSocket* socket);


private:
//    QTcpSocket *psocket;

    quint16 inDataSize;
    QFile *localFile;  //要发送的文件
    qint64 totalBytes;  //数据总大小
    qint64 bytesWritten;  //已经发送数据大小
    qint64 bytesToWrite;   //剩余数据大小,即文件实际内容的大小
    qint64 loadSize;   //每次发送数据的大小
    QString fileName;  //保存文件路径

    QByteArray outBlock;  //数据缓冲区，即存放每次要发送的数据
    quint16 blockSize;

    QString message;
    int authflags = 0;

    QString m_err;
    QStringList m_plist;
    QString m_pnameStr;
    quint16 printerNo;
    quint8 m_reConnFlg;
    QFileSystemWatcher *m_printerWatcher;
    QString m_sip;
    QString m_license;
    QStringList m_iplist;
    QStringList m_liclist;
    QList<ClientSockPtr> m_ClientSocks;
    static QMap<QString,ClientSockPtr> m_sockMap;
    QRegExp rx;
    QRegExp rx1;

};


#define client EPT::Client::instance()


}

#endif // CLIENT_H
