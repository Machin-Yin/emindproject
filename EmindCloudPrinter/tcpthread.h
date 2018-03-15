#ifndef TCPTHREAD_H
#define TCPTHREAD_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QFile>

namespace EPT {

class TcpThread : public QThread
{
    Q_OBJECT
public:
    TcpThread(QString& ip,QString& license,QStringList& printerAddrs,QObject* parent=0);
    void run();
    void searchPPD();
    void rmFiles();
    void connPrnServer(QString ip,QString license);
    void savConnection(QString printerlist);
    void getPrinterNameList(QString& rmsg);
    void sndMsg(QString msgStr);
    QString rcvMsg();
    QString pnameStr();
    void setPnameStr(const QString);
    QStringList printerNameList();
    void setPrinterNameList(const QStringList);
    void reqPrinterList();

signals:
    void error(QTcpSocket::SocketError sockErr);
    void error();
    void errChanged();
    void errConnected();
    void printerNameListChanged();
    void pNameListModelChanged(QStringList);
    void rcvCupsFile();
    void sigConnected();
    void sigAuthWrong();
    void sigConnectRefused();
    void pnameStrChanged();
    void plistSent();
    void printerSetFinished();
    void stopIndicator();
    void pnameStrSent(QString);
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


public slots:
    void displayError(QAbstractSocket::SocketError psocketErr);
//    void checkLicense();
    void resumeConnect();
    void sndLicense();
    void onReadyRead();
    void loadCupsFiles(const QStringList& fileNames,const QStringList& titles,const quint32 count,const QString& devUri,const QString& options,bool autoRemove);
    void sendFile(const QString fileName,const QString title,const quint32 count,const QString& devUri,const QString& options,bool autoRemove);  //实现文件大小等信息的发送



private:
    QTcpSocket *psocket;
    QString m_ip;
    QString m_license;
    QStringList m_plist;

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
    QString m_err;
    QString m_pnameStr;
    quint16 printerNo;
    quint8 m_reConnFlg;
};


}
#endif // TCPTHREAD_H
