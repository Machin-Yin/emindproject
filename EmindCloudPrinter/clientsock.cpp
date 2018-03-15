#include "clientsock.h"
#include "protocol.h"
#include <QDataStream>
#include <QDir>
#include <QThread>
#include "client.h"
#include <QString>
#include <QMap>

qint32 ClientSock::m_sameipNum = 0;
QString ClientSock::m_sameip = NULL;

ClientSock::ClientSock(QString& sip,QString& slicense):m_sip(sip),m_license(slicense)
{
    qDebug()<<__FUNCTION__<<endl;

    connect(this,SIGNAL(connected()),this,SLOT(sndlicense()));
    connect(this,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
    connect(this,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
    connect(this,SIGNAL(pnameStrRcved(QString,QString)),client,SIGNAL(pnameStrSent(QString,QString)));
    connect(this,SIGNAL(pnameStrRcved(QString,QString)),client,SLOT(savConnection(QString,QString)));
    connect(this,SIGNAL(sigConnected(QString)),client,SIGNAL(sigConnected(QString)));
    connect(this,SIGNAL(sigAuthWrong(QString)),client,SIGNAL(sigAuthWrong(QString)));
    connect(this,SIGNAL(sigConnectRefused(QString)),client,SIGNAL(sigConnectRefused(QString)));
    connect(client,SIGNAL(sigload(QStringList,QStringList,quint32,QString,QString,bool)),this,SLOT(loadCupsFiles(QStringList,QStringList,quint32,QString,QString,bool)));
    connect(this,SIGNAL(printerAdded(QString)),client,SIGNAL(printerAdded(QString)));
    connect(client,SIGNAL(findPPD()),this,SLOT(searchPPD()));
    connect(client,SIGNAL(sigDefPrinter(qint32)),this,SLOT(setDefPrinter(qint32)));


    qDebug()<<"this pointer="<<this<<endl;
    connectToHost(sip,SERVER_PORT);

    loadSize = 4*1024;
    totalBytes = 0;
    bytesWritten = 0;
    bytesToWrite = 0;
    blockSize = 0;
}

ClientSock::ClientSock()
{

}

ClientSock::~ClientSock()
{

}

void ClientSock::sndlicense()
{
    sndMsg("License");
    sndMsg(m_license);
}
void ClientSock::sndlicense(QString license)
{
    sndMsg("License");
    sndMsg(license);
}

void ClientSock::onReadyRead()
{
    QString rmsg=rcvMsg();
    qDebug() <<__FUNCTION__<<"Received Msg="<<rmsg<< endl;

    if(!rmsg.isNull())
    {
        if((rmsg.length()<5)&& (rmsg == "OK")){
            emit sigConnected(m_sip);
//            qApp->setQuitOnLastWindowClosed(false);
            sndMsg("Request printer list!");
        }else if(rmsg == "AUTH WRONG"){
            emit sigAuthWrong(m_sip);
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
    qDebug()<<__FUNCTION__<<"check license over"<<endl;
}

void ClientSock::sndMsg(QString msgStr)
{
    QByteArray authblock;
    QDataStream out(&authblock,QIODevice::WriteOnly);
    out << (quint16)0 << msgStr;
    out.device()->seek(0);
    out<<(quint16)(authblock.size() - sizeof(quint16));
//    qDebug()<<"psocket.state="<<this->state()<<endl;
    this->write(authblock);
    this->flush();
}

QString ClientSock::rcvMsg()
{
    QDataStream in(this);

    qDebug()<<__FUNCTION__<<this->bytesAvailable()<<endl;

    if(blockSize == 0 ){
        if(this->bytesAvailable()<(int)sizeof(quint16)){
            return 0;
        }

        in >> blockSize;
    }
    if (this->bytesAvailable() < blockSize)
    {
        return 0;
    }

    in >> message;
    blockSize = 0;
    return message;
}

void  ClientSock::getPrinterNameList(QString& rmsg)
{
    qDebug()<<__FUNCTION__<<endl;
    QString printers = rmsg;

    emit pnameStrRcved(m_sip,printers);

    QStringList tmpList;
    tmpList = printers.split(",");

    for(int i=0;i<tmpList.count();i++) {
        m_plist << tmpList[i];
    }

//    setPrinterNameList(m_plist);
//    emit plistSent();
//    savConnection(printers);             //when connected,save the ip and authcode

    qDebug( )<<"pnameStrRcved==>"<<endl;
}

void ClientSock::searchPPD()
{
    qDebug()<<__FUNCTION__<<endl;
    QDir dir(PPD_LOCATION);
    QStringList filelist;
    dir.setFilter(QDir::Files);
    filelist = dir.entryList();
    qDebug()<<"m_plist.size()="<<m_plist.size()<<endl;
    qDebug()<<"filelist.size="<<filelist.count()<<endl;
    for(qint32 i=0;i<filelist.count();i++){
        for(qint32 j=0;j<m_plist.count();j++){
            QString ppdName = m_plist.at(j);
            ppdName.replace(QRegExp(" "),"_");
            //        ppdName = ppdName + "@" + m_ip + ".ppd";//QRegExp(QString("%1@%2.ppd*").arg(ppdName).arg(m_ip)
            if(filelist.at(i).contains(QRegExp(QString("^%1@%2.ppd$").arg(ppdName).arg(m_sip)))){
                qDebug()<<filelist.at(i)<<"exist"<<endl;
                qDebug()<<__FUNCTION__<<"ppdName="<<(ppdName+QString("@%1").arg(m_sip))<<endl;
                emit printerAdded(ppdName+QString("@%1").arg(m_sip));
            }
        }
    }
}


void ClientSock::sendFiles(const QString fileName,const QString title,const quint32 count,const QString& devUri,const QString& options,bool autoRemove)  //实现文件大小等信息的发送
{
    qDebug()<<"file to print:"<<fileName<<"printer devUri="<<devUri<<endl;
    qDebug()<<__FUNCTION__<<endl;
    //devUri==>emindprinter://192.168.30.182/Microsoft_Print_to_PDF
    if(!m_plist.isEmpty()){
        qDebug()<<__FUNCTION__<<"m_plist is empty"<<endl;
        for(qint32 i=0;i<m_plist.count();i++){
            QString str = m_plist.at(i);
            str.replace(QRegExp(" "),QString("_"));
            if(devUri.split("/").at(3)== str ){
                qDebug()<<"selected printer="<<devUri.split("/").at(3)<<endl;
                qDebug()<<QString("m_plist.at(%1)=").arg(i)<<m_plist.at(i)<<endl;
                sndMsg("DefaultPrinter\r\n"+QString::number(i));
//                sndMsg(QString::number(i));
            }
        }
    }else{
        QString devAddr = devUri.split("/").at(3);
        sndMsg("DefaultPrinter\r\n"+devAddr.replace("_"," "));
//        sndMsg(devAddr.replace("_"," "));
    }

    sndMsg("CopyCount\r\n"+QString::number(count));
//    sndMsg(QString::number(count));
    sndMsg("begin send file");

    //foreach(auto fileName,Files){
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

    this->write(outBlock);
    //发送完头数据后剩余数据的大小，即文件实际内容的大小
    //    qDebug()<<"#####totalBytes"<<outBlock.size() - sizeof(qint64) * 2;

    this->waitForBytesWritten();
    outBlock.resize(0);
    qDebug()<<"sendfiles over!"<<endl;

}

/*
void ClientSock::sendFiles(const QString fileName,const QString title,const quint32 count,const QString& devUri,const QString& options,bool autoRemove)  //实现文件大小等信息的发送
{
    qDebug()<<"file to print:"<<fileName<<"printer devUri="<<devUri<<endl;
    qDebug()<<__FUNCTION__<<endl;
    //devUri==>emindprinter://192.168.30.182/Microsoft_Print_to_PDF
    if(!m_plist.isEmpty()){
        qDebug()<<__FUNCTION__<<"m_plist is empty"<<endl;
        for(qint32 i=0;i<m_plist.count();i++){
            QString str = m_plist.at(i);
            str.replace(QRegExp(" "),QString("_"));
            if(devUri.split("/").at(3)== str ){
                qDebug()<<"selected printer="<<devUri.split("/").at(3)<<endl;
                qDebug()<<QString("m_plist.at(%1)=").arg(i)<<m_plist.at(i)<<endl;
                sndMsg("DefaultPrinter\r\n"+QString::number(i));
//                sndMsg(QString::number(i));
            }
        }
    }else{
        QString devAddr = devUri.split("/").at(3);
        sndMsg("DefaultPrinter\r\n"+devAddr.replace("_"," "));
//        sndMsg(devAddr.replace("_"," "));
    }

    sndMsg("CopyCount\r\n"+QString::number(count));
//    sndMsg(QString::number(count));
    sndMsg("begin send file");

    //foreach(auto fileName,Files){
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

    this->write(outBlock);
    //发送完头数据后剩余数据的大小，即文件实际内容的大小
    //    qDebug()<<"#####totalBytes"<<outBlock.size() - sizeof(qint64) * 2;

    this->waitForBytesWritten();
    outBlock.resize(0);
    qDebug()<<"sendfiles over!"<<endl;

}*/

void ClientSock::displayError(QAbstractSocket::SocketError psocketErr)
{
    qDebug()<<__FUNCTION__<<endl;
    switch(psocketErr){
    case QAbstractSocket::HostNotFoundError:
        qDebug()<<"HostNotFoundError"<<endl;
        emit sigConnectRefused(m_sip);
        break;

    case QAbstractSocket::ConnectionRefusedError:
        qDebug()<<"ConnectionRefusedError"<<endl;
        emit sigConnectRefused(m_sip);
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
        emit sigConnectRefused(m_sip);
        break;
    case QAbstractSocket::RemoteHostClosedError:
        emit sigConnectRefused(m_sip);
        break;
    default:
        break;
    }
}

void ClientSock::setDefPrinter(qint32 pindex)
{
    sndMsg("DefaultPrinter");
    sndMsg(QString::number(pindex));
}

void ClientSock::loadCupsFiles(const QStringList& fileNames,const QStringList& titles,const quint32 count,const QString& devUri,const QString& options,bool autoRemove)
{
    qDebug()<<__FUNCTION__<<endl;
    qDebug()<<"m_sip="<<m_sip<<endl;
    qDebug()<<"devUri.split('/').at(2)="<<devUri.split("/").at(2)<<endl;
    if(m_sip == devUri.split("/").at(2)){
//        if(m_sockMap.values(m_sip).count()>1){
//            m_sameipNum = 1;
//        }
            for(qint32 i=0;i<fileNames.count();i++){
                sendFiles(fileNames.at(i),titles.at(i),count,devUri,options,autoRemove);
            }
        }
}
