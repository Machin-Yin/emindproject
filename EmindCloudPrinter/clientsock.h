#ifndef CLIENTSOCK_H
#define CLIENTSOCK_H
#include <QAbstractSocket>
#include <QObject>
#include <QTcpSocket>
#include <QFile>

class ClientSock : public QTcpSocket
{
    Q_OBJECT
public:
    ClientSock(QString& sip,QString& slicense);
    ClientSock();
    ~ClientSock();

    void sndlicense(QString license);


public slots:
    void sndlicense();
    void onReadyRead();
    void sndMsg(QString msgStr);
    QString rcvMsg();
    void getPrinterNameList(QString& rmsg);
    void sendFiles(const QString fileName,const QString title,const quint32 count,const QString& devUri,const QString& options,bool autoRemove);
    void displayError(QAbstractSocket::SocketError psocketErr);
    void setDefPrinter(qint32 pindex);
    void loadCupsFiles(const QStringList& fileNames,const QStringList& titles,const quint32 count,const QString& devUri,const QString& options,bool autoRemove);
    void searchPPD();


signals:
    void sigConnected(QString);
    void sigAuthWrong(QString);
    void sigPrintSuccess();
    void sigPrintFailed();
    void sigConnectRefused(QString);
    void pnameStrRcved(QString,QString);
    void printerAdded(QString);


private:

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
    QString m_sip;
    QString m_license;
    QStringList m_plist;
    static qint32 m_sameipNum;
    static QString m_sameip;
    static QSet<QString> m_ipSet;

};

#endif // CLIENTSOCK_H
