#ifndef PACKAGE_H
#define PACKAGE_H

#include <QObject>
#include <QFile>
class Package
{
public:
    virtual const char* load (const char* data);
    virtual char* dump (char* data, size_t size) const;

    virtual void show_debug();
    virtual ~Package();

    char		_verify[16];
    uint16_t	_command;
};

class LicensePackage:public Package
{
public:
    bool licOk;
    const char* load(const char* data);
    char* dump(char* data,size_t size) const;

};

class PrnlistPackage:public Package
{
public:
    QString m_printerlist;
    const char* load(const char* data);
    char* dump(char* data,size_t size) const ;
};

class FilePackage:public Package
{
public:
    QByteArray fileData;
    qint32 fileNameSize;
    QString fileName;
    QString devUri;
    qint32 devUriSize;
    qint32 fileSize;
    quint32 copies;
//    QString options;

    const char* load(const char* data);
    char* dump(char* data,size_t size) const;
private:

};

#endif // PACKAGE_H
