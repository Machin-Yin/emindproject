#include "package.h"
#include <string.h>
#include <windows.h>
//#include <arpa/inet.h>
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QDir>

qint32 my_copies;
QString my_prName;
QString my_tempFile;


const char* Package::load (const char* data)
{
    const char* p = data;

    _command = ntohs(*((quint16*)p));
    p += sizeof(quint16);

    memcpy(_verify, p, sizeof(_verify));
    p += sizeof(_verify);

    return p;
}

char* Package::dump (char* data, size_t size)
{
    if (size < (sizeof(_command) + sizeof(_verify)))
        return NULL;

    char* p = data;

    *((quint16*)data) = htons(_command);
    p += sizeof(quint16);

    memcpy(p, _verify, sizeof(_verify));
    p += sizeof(_verify);

    return p;
}

const char* PackPrinter::load(const char* data)
{
    const char* d_data = Package::load(data);
    m_printerlist = d_data;
    return d_data;
}


char* PackPrinter::dump (char* data, size_t size)
{
    if (size < (sizeof(_command) + sizeof(_verify)))
        return NULL;

    char* p = Package::dump(data, size);
    size -= (p - data);

    const std::string& str = m_printerlist.toStdString();
    memcpy(p, str.c_str(), str.size());
    p += str.size();

    return p;
}

const char* PackFile::load(const char* data)
{
    const char *p = Package::load(data);

    copies = ntohl(*((qint32*)p));
    p += sizeof(qint32);
    qDebug() << "copies()==" << copies << endl;

    prNameSize = ntohl(*((qint32*)p));
    p += sizeof(qint32);
    qDebug() << "prNameSize()==" << prNameSize << endl;

    char* prNamebuf = new char[prNameSize];
    memcpy(prNamebuf,p,prNameSize);
    char* prNameBuf = new char[prNameSize/2];
    for(int i=0;i<prNameSize/2;i++){
        prNameBuf[i] = prNamebuf[2*i];
    }
    prName = QString::fromLocal8Bit(prNameBuf,prNameSize/2);
    qDebug()  << "prName==" << prName <<endl;
     p += prNameSize;

    fileNameSize = ntohl(*((qint32*)p));
    p += sizeof(qint32);
    qDebug() << "fileNameSize()==" << fileNameSize << endl;

    char* fileNamebuf = new char[fileNameSize];
    memcpy(fileNamebuf,p,fileNameSize);
    char* fileNameBuf = new char[fileNameSize/2];
    for(int j=0;j<fileNameSize/2;j++){
        fileNameBuf[j] = fileNamebuf[2*j];
    }
    fileName = QString::fromLocal8Bit(fileNameBuf,fileNameSize/2);
    qDebug()  << "fileName==" << fileName <<endl;
    p += fileNameSize;

    fileSize = ntohl(*((qint32*)p));
    p += sizeof(qint32);
    qDebug() << "fileSize()==" << fileSize << endl;

    tempFolder = QDir::tempPath();
    if(QSysInfo::windowsVersion() == QSysInfo::WV_XP)
    {
        tempFile = fileName;
    }
    else
    {
        tempFile = tempFolder + "/" + fileName;
    }
    char* pfile = new char[fileSize];
    memcpy(pfile,p,fileSize);
    localFile = new QFile(tempFile);
    if (!localFile->open(QIODevice::ReadWrite))
    {
        qDebug() << "open file error!";
        return p;
    }
    else
    {
        localFile->write(pfile,fileSize);
        if(localFile->flush())
            localFile->close();
        else
            localFile->waitForBytesWritten(3000);
        qDebug() << "receive" << fileName << "over!" << endl;
        qDebug() << "Save " << tempFile << " over!" << endl;

    }

    my_copies = copies;
    my_prName = prName;
    my_tempFile = tempFile;

    return p;
}


char* PackFile::dump (char* data, size_t size)
{
    if (size < (sizeof(_command) + sizeof(_verify)))
        return NULL;

    char* p = data;

    *((quint16*)data) = htons(_command);
    p += sizeof(quint16);

    memcpy(p, _verify, sizeof(_verify));
    p += sizeof(_verify);

    return p;
}

void Package::show_debug()
{
    qDebug()<<"command = "<<_command<< "verify = "<< _verify<<endl;
}

