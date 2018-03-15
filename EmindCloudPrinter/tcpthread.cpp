#include "tcpthread.h"
#include <QDir>
#include "protocol.h"
#include <QApplication>
#include <QDataStream>
#include <QSettings>
#include <QStandardPaths>
#include <QMessageBox>

namespace EPT {
TcpThread::TcpThread(QString& ip,QString& license,QStringList& printerAddrs,QObject* parent):QThread(parent),m_ip(ip),m_license(license),m_plist(printerAddrs)
{
    qDebug()<<"m_plist.size="<<m_plist.size()<<endl;
    loadSize = 4*1024;
    totalBytes = 0;
    bytesWritten = 0;
    bytesToWrite = 0;
    blockSize = 0;
    m_reConnFlg = 0;
    qDebug()<<__FUNCTION__<<" current thread is "<<QThread::currentThread()<<endl;


}

void TcpThread::run()
{
//    rmFiles();
//    searchPPD();
    qDebug()<<__FUNCTION__<<" current thread is "<<QThread::currentThread()<<endl;

    connPrnServer(m_ip,m_license);
    exec();

}

void TcpThread::connPrnServer(QString ip,QString license)
{
    qDebug()<<__FUNCTION__<<" current thread is "<<QThread::currentThread()<<endl;

    psocket = new QTcpSocket;
    connect(psocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
    connect(psocket,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
    connect(psocket,SIGNAL(disconnected()),this,SLOT(resumeConnect()));
    connect(psocket,SIGNAL(connected()),this,SLOT(sndLicense()));
    psocket->close();
    psocket->connectToHost(ip,SERVER_PORT);
//    if(psocket->waitForConnected(1000)){
//        if(!m_reConnFlg){
//            sndMsg("License");
//            sndMsg(license);
//        }
//    }
//    else{
//        emit sigConnectRefused();
//        qDebug()<<"connected failed"<<endl;
//    }

//    psocket->waitForReadyRead();
//    while(psocket->waitForReadyRead());
//    if (psocket->waitForReadyRead())
//        onReadyRead();

    qDebug()<<__FUNCTION__<<"connected over"<<endl;
}

void TcpThread::sndLicense()
{
    if(!m_reConnFlg){
        sndMsg("License");
        sndMsg(m_license);
    }else{
        emit sigConnectRefused();
        qDebug()<<"connected failed"<<endl;
    }
}

void TcpThread::onReadyRead()
{
    qDebug()<<__FUNCTION__<<" current thread is "<<QThread::currentThread()<<endl;

    QString rmsg=rcvMsg();
    qDebug() <<__FUNCTION__<<"Received Msg="<<rmsg<< endl;

    if(!rmsg.isNull())
    {
        if((rmsg.length()<5)&& (rmsg == "OK")){
            emit sigConnected();
            qApp->setQuitOnLastWindowClosed(false);
//            sndMsg("Request printer list!");
            reqPrinterList();
        }else if(rmsg == "AUTH WRONG"){
            emit sigAuthWrong();
        }else if(rmsg.contains("Printlist\r\n") ){
            QString tmpMsg = rmsg.mid(rmsg.indexOf(QString("\r\n"))+2);
            getPrinterNameList(tmpMsg);
        }else if(rmsg =="PrintSuccess"){
            qDebug()<<"Printer Success"<<endl;
            emit sigPrintSuccess();
        }else if(rmsg == "PrintFailure"){
            qDebug()<<"Printer Failed"<<endl;
            emit sigPrintFailed();
        }
    }
}

void TcpThread::reqPrinterList()
{
    qDebug()<<__FUNCTION__<<endl;
    sndMsg("Request printer list!");
}

void  TcpThread::getPrinterNameList(QString& rmsg)
{
    qDebug()<<__FUNCTION__<<endl;
    QString printers = rmsg;

    setPnameStr(printers);

    QStringList tmpList;
    tmpList = printers.split(",");

    for(int i=0;i<tmpList.count();i++) {
        m_plist << tmpList[i];
    }

    setPrinterNameList(m_plist);
    emit plistSent();
    if(1 == m_reConnFlg)
        searchPPD();

    savConnection(printers);             //when connected,save the ip and authcode

    qDebug()<<"plistSent==>"<<endl;
}

QString TcpThread::pnameStr()
{
    return m_pnameStr;
}

void TcpThread::setPnameStr(const QString str)
{
    m_pnameStr = str;
    emit pnameStrSent(m_pnameStr);
    emit pnameStrChanged();
}

void TcpThread::setPrinterNameList(const QStringList list)
{
    m_plist = list;
    emit printerNameListChanged();
}

QStringList TcpThread::printerNameList()
{
    return m_plist;
}

void TcpThread::sndMsg(QString msgStr)
{
    qDebug()<<"current thread is "<<QThread::currentThread()<<endl;
//    qDebug()<<__FUNCTION__<<this<<endl;
    QByteArray authblock;
    QDataStream out(&authblock,QIODevice::WriteOnly);
    out << (quint16)0 << msgStr;
    out.device()->seek(0);
    out<<(quint16)(authblock.size() - sizeof(quint16));
//    qDebug()<<"psocket.state="<<psocket->state()<<endl;
    psocket->write(authblock);
    psocket->flush();
}

// Receive short messages : like protocol
QString TcpThread::rcvMsg()
{
    QDataStream in(psocket);

    qDebug()<<psocket->bytesAvailable()<<endl;

    if(blockSize == 0 ){
        if(psocket->bytesAvailable()<(int)sizeof(quint16)){
            return 0;
        }

        in >> blockSize;
    }
    if (psocket->bytesAvailable() < blockSize)
    {
        return 0;
    }

    in >> message;
    blockSize = 0;
    qDebug()<<"message"<<message<<endl;
    return message;
}

void TcpThread::loadCupsFiles(const QStringList& fileNames,const QStringList& titles,const quint32 count,const QString& devUri,const QString& options,bool autoRemove)
{
    qDebug()<<__FUNCTION__<<endl;
    for(quint32 i=0;i<fileNames.count();i++)
    {
        sendFile(fileNames.at(i),titles.at(i),count,devUri,options,autoRemove);
    }
}


void TcpThread::sendFile(const QString fileName,const QString title,const quint32 count,const QString& devUri,const QString& options,bool autoRemove)  //实现文件大小等信息的发送
{
    qDebug()<<"file to print:"<<fileName<<"printer devUri="<<devUri<<endl;

    qDebug()<<__FUNCTION__<<endl;
    //devUri==>emindprinter://192.168.30.182/Microsoft_Print_to_PDF
    if(m_plist.isEmpty()){
        for(quint32 i=0;i<m_plist.count();i++){
            QString str = m_plist.at(i);
            str.replace(QRegExp(" "),QString("_"));
            if(devUri.split("/").at(3)== str ){
                qDebug()<<"selected printer="<<devUri.split("/").at(3)<<endl;
                qDebug()<<QString("m_plist.at(%1)=").arg(i)<<m_plist.at(i)<<endl;
                sndMsg("DefaultPrinter");
                sndMsg(QString::number(i));
            }
        }
    }else{
        qDebug()<<"m_plist="<<m_plist<<endl;
        QString devAddr = devUri.split("/").at(3);
        sndMsg("DefaultPrinter");
        sndMsg(devAddr.replace("_"," "));
    }

    sndMsg("CopyCount");
    sndMsg(QString::number(count));

    sndMsg("begin send file");

    //    foreach(auto fileName,Files){
    localFile = new QFile(fileName);
    if(!localFile->open(QFile::ReadOnly))
    {
        qDebug() << "open file error!";
        return;
    }
    //        fileName = localFile->fileName();
    //文件总大小
    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);  //将发送缓冲区封闭在一个QDataStream类型的变量中
    QString currentFileName = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);

    //依次写入总大小信息空间，文件名大小信息空间，文件名---仅是文件名不含路径，没必要含路径
    totalBytes += outBlock.size();

    sendOut << qint64(0) << qint64(0) << currentFileName;
    qint64 filename_size = outBlock.size() - sizeof(qint64) * 2;
    //这里的总大小是文件名大小等信息和实际文件大小的总和
    outBlock.append(localFile->readAll());

    sendOut.device()->seek(0);
    sendOut<<qint64((outBlock.size() - sizeof(qint64)*2));
    sendOut<<qint64(filename_size);

    psocket->write(outBlock);
    //发送完头数据后剩余数据的大小，即文件实际内容的大小
    //    qDebug()<<"#####totalBytes"<<outBlock.size() - sizeof(qint64) * 2;

    psocket->waitForBytesWritten();
    outBlock.resize(0);
    qDebug()<<"sendfiles over!"<<endl;

}


void TcpThread::searchPPD()
{
    QDir dir(PPD_LOCATION);
    QStringList filelist;
    dir.setFilter(QDir::Files);
    filelist = dir.entryList();
    qDebug()<<"filelist.size="<<filelist.count()<<endl;
    for(qint32 i=0;i<filelist.count();i++){
        for(qint32 j=0;j<m_plist.count();j++){
            QString ppdName = m_plist.at(j);
            ppdName.replace(QRegExp(" "),"_");
            //        ppdName = ppdName + "@" + m_ip + ".ppd";//QRegExp(QString("%1@%2.ppd*").arg(ppdName).arg(m_ip)
            if(filelist.at(i).contains(QRegExp(QString("^%1@%2.ppd$").arg(ppdName).arg(m_ip)))){
                qDebug()<<filelist.at(i)<<"exist"<<endl;
                emit printerAdded(ppdName);
            }
        }
    }
}


void TcpThread::rmFiles()
{
    QDir dir(QDir::homePath()+"/.cache");
    QStringList filelist;
    QStringList namefilters;
    namefilters<<"emindprinter*.pdf";
    dir.setNameFilters(namefilters);
    dir.setFilter(QDir::Files);
    filelist = dir.entryList();
    if(filelist.count()>10)
    foreach (auto file, filelist) {
        qDebug()<<"file to remove"<<file<<endl;
        QFile::remove(QDir::homePath()+"/.cache/"+file);
    }

}

void TcpThread::savConnection(QString printerlist)
{
    qDebug()<<__FUNCTION__<<endl;
    qDebug()<<"m_ip="<<m_ip<<endl;
    qDebug()<<"m_license="<<m_license<<endl;
    if(psocket->state() == QAbstractSocket::ConnectedState){
        QSettings settings(CONFFile,QSettings::NativeFormat);
        //        settings.beginGroup(priver->serverIp);
        settings.setValue("PrinterIP",m_ip);
        settings.setValue("AuthCode",m_license);
        settings.setValue("PrinterList",printerlist);
        //        settings.endGroup();
        qDebug()<<"saving connection"<<endl;
    }
}

void TcpThread::displayError(QAbstractSocket::SocketError psocketErr)
{
    qDebug()<<__FUNCTION__<<endl;
    switch(psocketErr){
    case QAbstractSocket::HostNotFoundError:
        qDebug()<<"HostNotFoundError"<<endl;
        emit sigConnectRefused();
        break;

    case QAbstractSocket::ConnectionRefusedError:
        qDebug()<<"ConnectionRefusedError"<<endl;
        emit sigConnectRefused();
        break;
    case QAbstractSocket::SocketTimeoutError:
        qDebug()<<"psocketTimeoutError"<<endl;
        //        emit sigConnectRefused();
        break;
    case QAbstractSocket::DatagramTooLargeError:
        qDebug()<<"DatagramTooLargeError"<<endl;
        //        emit sigConnectRefused();
        break;
    case QAbstractSocket::NetworkError:
        emit sigConnectRefused();
        break;
    case QAbstractSocket::RemoteHostClosedError:
        emit sigConnectRefused();
        break;
    default:
        break;
    }
}

void TcpThread::resumeConnect()
{
    QMessageBox::warning(0,tr("EmindCloudPrinter"),tr("ERROR:Disconnected From Server,Please Restart!"));
    //    emit sigOffline();
}










}
