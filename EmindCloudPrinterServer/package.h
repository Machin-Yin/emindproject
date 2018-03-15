#ifndef PACKAGE_H
#define PACKAGE_H

#include <QObject>
#include <QFile>

extern qint32 my_copies;
extern QString my_prName;
extern QString my_tempFile;
extern QString savedAuthCode;


class Package
{
public:
  virtual const char* load (const char* data);
  virtual char* dump (char* data, size_t size);

  virtual void show_debug();

  char		_verify[16];
  quint16	_command;
};

class PackPrinter:public Package
{
public:
    QString m_printerlist;
    const char* load(const char* data);
    char* dump(char* data,size_t size);
};

class PackFile:public Package
{
public:
    qint32 copies;
    qint32 prNameSize;
    QString prName;
    qint32 fileNameSize;
    QString fileName;
    qint32 fileSize;
    QString tempFolder;
    QString tempFile;
    QFile *localFile;


    const char* load(const char* data);
    char* dump(char* data,size_t size);

};



#endif // PACKAGE_H
