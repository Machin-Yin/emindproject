#include "client.h"
#include <QTcpSocket>
#include <QProcess>
#include "printerlistmodel.h"
#include "tcpthread.h"
#include <QFile>
#include <QFileSystemWatcher>
#include <QDBusConnection>
#include "clientsock.h"
//#include <utility>
#include <QMap>
#include "package.h"
#include <arpa/inet.h>
#include <packagecreate.h>
#include "string.h"
#include <stdlib.h>
#include <QBuffer>
#include <QAbstractSocket>
#include <QMessageBox>
#include <QTimer>

namespace EPT {

QMap<QString,ClientSockPtr>  Client::m_sockMap;

Client::Client(QObject *parent) : QObject(parent)
{
    new ClienterInterfaceAdaptor(this);
    QDBusConnection dbusConn = QDBusConnection::sessionBus();
    bool ret = dbusConn.registerService("org.emindprinter.emgui");
    if(ret)
        qDebug()<<__FUNCTION__<<"register service success"<<endl;
    else
        qDebug()<<__FUNCTION__<<"register service failed"<<endl;
    ret = dbusConn.registerObject("/Clienter",this,QDBusConnection::ExportAdaptors);
    if(ret)
        qDebug()<<__FUNCTION__<<"register object success"<<endl;
    else
        qDebug()<<__FUNCTION__<<"register object failed"<<endl;

    m_printerWatcher = new QFileSystemWatcher(this);
    connect(m_printerWatcher,SIGNAL(fileChanged(QString)),this,SIGNAL(sigPrnDeleted(QString)));
    connect(m_printerWatcher,SIGNAL(fileChanged(QString)),this,SLOT(prnDelete(QString)));
    watchPrinters();
//    connect(this,SIGNAL(rmfiles()),this,SLOT(rmFiles()));
    //    rmFiles();
    rx = QRegExp("\\s");
    rx1 = QRegExp("/");
}

Client::~Client()
{
}



Client* Client::instance()
{
    static Client *inst = 0;
    if(!inst)
        inst = new Client();
    return inst;
}

void Client::prnDelete(QString prnDel)
{
    if(!QFile(prnDel).exists())
        qDebug()<<"printer deleted: "<<prnDel<<endl;
    else
        qDebug()<<"printer added: "<<prnDel<<endl;
}

void Client::onConnServer(QString ip,QString license)
{
    m_sip = ip;
    m_license = license;
    qDebug()<<__FUNCTION__<<"m_sip="<<m_sip<<endl;
    qDebug()<<__FUNCTION__<<"m_license="<<m_license<<endl;
    //    createConnection(ip,license);

    connToServer(ip,license);

}


void Client::connToServer(QString ip,QString license)
{
    qDebug()<<__FUNCTION__<<"ip="<<ip<<"license="<<license<<endl;
    if(checkLicense(ip,license)){
        m_license = license;
        m_sip = ip;
        savConnection(ip);

        if(!getPrnlist(ip,license).isEmpty()){
            emit sigConnected(ip);
            emit pnameStrSent(ip,m_pnameStr);
            QStringList tmpList;
            tmpList = m_pnameStr.split(",");

            for(int i=0;i<tmpList.count();i++) {
                m_plist << tmpList[i];
            }
        }
        qDebug()<<"m_pnameStr="<<m_pnameStr<<endl;
    }
}

bool Client::sendPackage(QTcpSocket* eclient, const Package* package, qint64 size)
{
    bool ret = false;
    if (eclient->waitForConnected(2000)) {
        char* buffer = new char[size * 2];
        char* p = buffer + sizeof(uint32_t);// p->buffer[2];

        p = package->dump(p, size - sizeof(uint32_t));
        char* org = buffer;
        *((uint32_t*)org) = htonl((uint32_t)(p - buffer - sizeof(uint32_t)));


        qint64 length = p - buffer;
        char* p1 = buffer;
        while (length) {
            qDebug() << "size = " << size << "length" << length << endl;
            qint64 number = eclient->write(p1, length);
             qDebug() << "number = " << number << endl;
            if (number <= 0)
                break;
            length -= number;
            p1 += number;
        }
        ret = eclient->waitForBytesWritten(1000);
        delete[] buffer;
    } else
        emit sigConnectRefused(m_sip);


    return ret;
}

Package* Client::readPackage(QTcpSocket* eclient)
{
    char buf[4];
    char* p = buf;
    Package* package = NULL;
    if (eclient->waitForReadyRead(2000)) {
        eclient->read(p, sizeof(buf));
        uint32_t size = ntohl(*((uint32_t*)(void*)p));
        QByteArray data = eclient->read(size);

        package = create_package_from_data(data.data());
        if (package){
            if(package->_verify[0] == '0') {
                emit sigAuthWrong(m_sip);
//                QMessageBox::warning(0,tr("emindprinter"),tr("license errror"));
                qDebug()<<"license error"<<endl;
                delete package;
                package = NULL;

//            }else if(package->_verify[0] == '1'){

//            }else if(package->_verify[0] == 'O'){

//            }else if(package->_verify[0] == 'F'){

            }
        }

    }
    return package;
}

bool Client::checkLicense(QString ip,QString license)
{
    bool rest = false;
    char buf[4];
    //    char* p = buf;
    QTcpSocket* eclient = new QTcpSocket();
    connect(eclient,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
    connect(eclient,SIGNAL(disconnected()),eclient,SLOT(deleteLater()));
    eclient->connectToHost(ip, SERVER_PORT);

    Package package;

    package._command = VERIFY_PACKAGE;
    memcpy(package._verify, license.toLatin1().data(), sizeof(package._verify));

    rest = sendPackage(eclient, &package, 1024);

    Package* retPackage = readPackage(eclient);

    rest &= (retPackage != NULL);
    delete retPackage;

    eclient->disconnectFromHost();
    eclient->waitForDisconnected(1000);
    delete eclient;
    //    delete packager;

    qDebug()<<__FUNCTION__<<"sendmessage finished"<<endl;
    return rest;

}

QString Client::getPrnlist(QString ip,QString license)
{
    QTcpSocket* eclient = new QTcpSocket();
    connect(eclient,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
    connect(eclient,SIGNAL(disconnected()),eclient,SLOT(deleteLater()));
    eclient->connectToHost(ip, SERVER_PORT);
    Package package;

    package._command = PRINTER_LIST;
    memcpy(package._verify, license.toLatin1().data(), sizeof(package._verify));

    if (sendPackage(eclient, &package, 1024)) {
        Package* retPackage = readPackage(eclient);
        if (retPackage) {
            PrnlistPackage* pkPrn = dynamic_cast<PrnlistPackage*>(retPackage);
            if(pkPrn)
                m_pnameStr = pkPrn->m_printerlist;
        }
    }


    eclient->disconnectFromHost();
    eclient->waitForDisconnected(1000);

    delete eclient;

    return m_pnameStr;
}

bool Client::sndFile(const QString& filePath,const QString& title,const quint32 count,const QString& devUri,const QString& options,bool autoRemove)
{
    qDebug()<<__FUNCTION__<<endl;
    bool rest = false;
    char buf[4];
    //    char* p = buf;
    QString sip = devUri.split("/").at(2);
    m_sip = sip;
    qDebug()<<__FUNCTION__<<"sip="<<sip<<endl;

    QFile fileToSnd(filePath);
    if(!fileToSnd.open(QFile::ReadOnly))
    {
        qDebug()<<__FUNCTION__<<"file open error"<<endl;
    }
    QString license = readConfig(sip);
    QTcpSocket* eclient = new QTcpSocket();
    connect(eclient,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
    eclient->connectToHost(sip, SERVER_PORT);
        FilePackage package;
        package._command = PRINT_FILE;
        memcpy(package._verify, license.toLatin1().data(), sizeof(package._verify));
        package.fileName = filePath.right(filePath.size() - filePath.lastIndexOf('/')-1);
        package.fileNameSize = 2*package.fileName.length();
        package.fileSize = fileToSnd.size();
        //        package.fileData = fileToSnd.read(package.fileSize);
        package.fileData = fileToSnd.readAll();
        fileToSnd.flush();
        fileToSnd.close();
        package.copies = count;
        package.devUri = devUri.split("/").at(3);
        package.devUri = package.devUri.replace("_"," ");
        package.devUri = package.devUri.replace("*","/");
        package.devUriSize = 2*package.devUri.length();
        //        package.options = options;
        qint64 totalSize = sizeof(package.fileNameSize) + package.fileNameSize + sizeof (package.fileSize) + package.fileSize + sizeof(package.copies) + sizeof (package._command) + sizeof (package._verify) + sizeof (package.devUriSize) + package.devUriSize;



        rest = sendPackage(eclient, &package, totalSize);

        Package* retPackage = readPackage(eclient);

        rest &= (retPackage != NULL);
        delete retPackage;
        eclient->disconnectFromHost();
        eclient->waitForDisconnected(600000);
        delete eclient;
//        cancelJob(filePath);
        QTimer::singleShot(2000,this,SLOT(rmFiles()));

    return rest;

}

void Client::releaseJob(QString fileToPrint)
{
    //    lp -i job-id -H resume
    QRegExp regex("[^\\d]+");
    QString jobidStr = fileToPrint.remove(regex);
    qDebug()<<__FUNCTION__<<jobidStr<<endl;

    qint32 jobid = jobidStr.toInt();
    qDebug()<<__FUNCTION__<<jobid<<endl;

    QProcess proc;
    proc.start(QString("lp -i  %1 -H resume").arg(jobid));
    proc.waitForFinished();

}

void Client::cancelJob(QString fileToPrint)
{
    QRegExp regex("[^\\d]+");
    QString jobidStr = fileToPrint.remove(regex);
    qDebug()<<__FUNCTION__<<jobidStr<<endl;

    qint32 jobid = jobidStr.toInt();
    qDebug()<<__FUNCTION__<<jobid<<endl;

    QProcess proc;
    proc.start(QString("lprm %1").arg(jobid));
    proc.waitForFinished();

}



void Client::createConnection(QString ip,QString license)
{
    qDebug()<<__FUNCTION__<<endl;
    if(!m_sockMap.count(ip)){
        qDebug()<<"new"<<endl;
        ClientSockPtr csock = ClientSockPtr(new ClientSock(ip,license));
        m_sockMap.insert(ip,csock);
    }else{
        //        m_sockMap.value(ip).data()->abort();
        if(!m_sockMap.value(ip).data()->socketDescriptor()){
            m_sockMap.value(ip).data()->deleteLater();
            m_sockMap.remove(ip);
            ClientSockPtr csock = ClientSockPtr(new ClientSock(ip,license));
            m_sockMap.insert(ip,csock);
            m_sockMap.value(ip).data()->connectToHost(ip,SERVER_PORT);
        }
        m_sockMap.value(ip).data()->sndlicense(license);
    }


}


void Client::load(const QString& fileName,const QString& title,const quint32 count,const QString& devUri,const QString& options,bool autoRemove)
{
    qDebug()<<__FUNCTION__<<endl;
    qDebug()<<"filename="<<fileName<<"   title="<<title<<"   count="<<count<<"  devUri="<<devUri<<"   options="<<options<<"   autoRemove="<<autoRemove<<endl;
    loadCupsFiles(QStringList()<<fileName,QStringList()<<title,count,devUri,options,autoRemove);

}


void Client::loadCupsFiles(const QStringList& fileNames,const QStringList& titles,const quint32 count,const QString& devUri,const QString& options,bool autoRemove)
{
    qDebug()<<__FUNCTION__<<endl;
    for(quint32 i=0;i<fileNames.count();i++)
    {
        sndFile(fileNames.at(i),titles.at(i),count,devUri,options,autoRemove);
        //        sendFiles(fileNames.at(i),titles.at(i),count,devUri,options,autoRemove);
    }

}

void Client::resumeConnect()
{
    QMessageBox::warning(0,tr("EmindCloudPrinter"),tr("ERROR:Disconnected From Server,Please Restart!"));
    //    emit sigOffline();
}

void Client::setDefaultPrinter(QString serverIp,QString prName,qint32 pIndex)
{
    qDebug()<<"serverIp="<<serverIp<<endl;
    //    QRegExp rx("(\\s|/)");

    if(!serverIp.isEmpty())
    {
        prName.replace(rx,"_");
        prName.replace(rx1,"*");
        QString ppdFileName=prName+"@"+serverIp + ".ppd";
        qDebug()<<"pIndex="<<pIndex<<endl;
        emit sigDefPrinter(pIndex);
        emit printerSetFinished();
        if(!QFile(PPD_LOCATION + ppdFileName).exists()){
            QProcess proc1;
            //        QString ppdName("/usr/share/ppd/emindprinter/emindprinter.ppd");
            QString ppdName("lsb/usr/emindprinter/emindprinter.ppd");
            //        proc1.start(QString("/usr/bin/emlpadmin  -p  %1@%2 -E -v emindprinter:/ -P %3  -o printer-is-shared=no -o PageSize=a4 ").arg(prName).arg(priver->serverIp).arg(ppdName));

            proc1.start(QString("/usr/bin/emlpadmin  -p  %1@%2 -E -v emindprinter://%2/%1 -m %3  -o printer-is-shared=no -o PageSize=a4 -o printer-error-policy=retry-job ").arg(prName).arg(serverIp).arg(ppdName));
            qDebug()<<"prName="<<prName<<endl;
            proc1.waitForFinished();

            QProcess proc2;
            proc2.start(QString("/usr/bin/emlpadmin -h localhost -d %1@%2").arg(prName).arg(serverIp));
            proc2.waitForFinished();
            m_printerWatcher->addPath(PPD_LOCATION + ppdFileName );
        }
    }
}

void Client::savConnection(QString sip)
{
    qDebug()<<__FUNCTION__<<endl;
    QSettings settings(CONFFile,QSettings::NativeFormat);
    settings.beginGroup(sip);
    settings.setValue("PrinterIP",sip);
    settings.setValue("AuthCode",m_license);
    settings.endGroup();
}

void Client::savConnection(QString sip,QString printerlist)
{
    qDebug()<<__FUNCTION__<<endl;
    QSettings settings(CONFFile,QSettings::NativeFormat);
    settings.beginGroup(m_sip);
    settings.setValue("PrinterIP",m_sip);
    settings.setValue("AuthCode",m_license);
    settings.setValue("PrinterList",printerlist);
    settings.endGroup();
}

QString Client::readConfig(QString ip)
{
    QSettings settings(CONFFile,QSettings::NativeFormat);
    QStringList groups = settings.childGroups();
    foreach(auto group,groups){
        qDebug()<<"group="<<group<<endl;
        if(group == ip){
            m_license = settings.value(QString("%1/AuthCode").arg(group)).toString();
            qDebug()<<"m_license="<<m_license<<endl;
        }
    }
    return m_license;
}

void Client::restore()
{
    qDebug()<<__FUNCTION__<<endl;
    QSettings settings(CONFFile,QSettings::NativeFormat);
    QStringList groups = settings.childGroups();
    foreach(auto group,groups){
        qDebug()<<"group="<<group<<endl;
        QString tmplicense = settings.value(QString("%1/AuthCode").arg(group)).toString();
        qDebug()<<"tmplicense="<<tmplicense<<endl;
        onConnServer(group,tmplicense);
    }
    //    QString tmpStr = settings.value("PrinterList").toString();
    //    qDebug()<<"tmpStr="<<tmpStr<<endl;
    //    QStringList tmplist;
    //    tmplist = tmpStr.split(",");
}

void Client::searchPPD()
{
    qDebug()<<__FUNCTION__<<endl;
    QDir dir(PPD_LOCATION);
    QStringList filelist;
    //    dir.setFilter();
    filelist = dir.entryList(QDir::Files);
    qDebug()<<"m_plist.size()="<<m_plist.size()<<endl;
    qDebug()<<"filelist.size="<<filelist.count()<<endl;
    for(qint32 i=0;i<filelist.count();i++){
        for(qint32 j=0;j<m_plist.count();j++){
            QString ppdName = m_plist.at(j);
            ppdName.replace(rx,"_");
            ppdName.replace(rx1,"*");
            //        ppdName = ppdName + "@" + m_ip + ".ppd";//QRegExp(QString("%1@%2.ppd*").arg(ppdName).arg(m_ip)
            //            QRegExp rx2(QString("%1@%2.ppd").arg(ppdName).arg(m_sip));
            if(filelist.at(i) == QString("%1@%2.ppd").arg(ppdName).arg(m_sip)){
                qDebug()<<filelist.at(i)<<"exist"<<endl;
                qDebug()<<__FUNCTION__<<"ppdName="<<(ppdName+QString("@%1").arg(m_sip))<<endl;
                emit printerAdded(ppdName+QString("@%1").arg(m_sip));
            }
        }
    }
}

void Client::watchPrinters()
{
    qDebug()<<__FUNCTION__<<endl;
    QDir dir(PPD_LOCATION);
    QFileInfoList fileInfolist;
    dir.setFilter(QDir::Files);
    fileInfolist = dir.entryInfoList();
    foreach (auto ppdfile, fileInfolist) {
        qDebug()<<"ppdfile path: "<<ppdfile.absoluteFilePath()<<endl;
        m_printerWatcher->addPath(ppdfile.absoluteFilePath());
    }


    //    for(qint32 i=0;i<filelist.count();i++){
    //        for(qint32 j=0;j<m_plist.count();j++){
    //            QString ppdName = m_plist.at(j);
    //            ppdName.replace(QRegExp(" "),"_");
    //            //        ppdName = ppdName + "@" + m_ip + ".ppd";//QRegExp(QString("%1@%2.ppd*").arg(ppdName).arg(m_ip)
    //            if(filelist.at(i).contains(QRegExp(QString("^%1@%2.ppd$").arg(ppdName).arg(m_sip)))){
    //                qDebug()<<filelist.at(i)<<"exist"<<endl;
    //                emit printerAdded(ppdName);
    //            }
    //        }
    //    }
}


//remove the rubbish files
void Client::rmFiles()
{
    QDir dir(QDir::homePath()+"/.cache");
    QStringList filelist;
    QStringList namefilters;
    namefilters<<"emindprinter*.pdf";
    dir.setNameFilters(namefilters);
    dir.setFilter(QDir::Files);
    filelist = dir.entryList();
    if(filelist.count()>3)
        foreach (auto file, filelist) {
            qDebug()<<"file to remove"<<file<<endl;
            QFile::remove(QDir::homePath()+"/.cache/"+file);
        }

}

void Client::displayError(QAbstractSocket::SocketError psocketErr)
{
    qDebug()<<"displayError m_sip===="<<m_sip<<endl;
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
        emit sigConnectRefused(m_sip);
        break;
    case QAbstractSocket::DatagramTooLargeError:
        qDebug()<<"DatagramTooLargeError"<<endl;
        emit sigConnectRefused(m_sip);
        break;
    case QAbstractSocket::NetworkError:
        qDebug()<<"NetworkError"<<endl;
        emit sigConnectRefused(m_sip);
        break;
    case QAbstractSocket::RemoteHostClosedError:
        qDebug()<<"RemoteHostClosedError"<<endl;
        emit sigConnectRefused(m_sip);
        break;
    default:
        qDebug()<<psocketErr<<endl;
        emit sigConnectRefused(m_sip);
        break;
    }
}


}
