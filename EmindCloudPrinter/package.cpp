#include "package.h"
#include <string.h>
#include <arpa/inet.h>
#include <QDebug>

Package::~Package()
{

}

//load received data and translate them to host sequence
const char* Package::load (const char* data)
{
    const char* p = data;

    _command = ntohs(*((uint16_t*)p));
    p += sizeof(uint16_t);

    memcpy(_verify, p, sizeof(_verify));
    p += sizeof(_verify);

    return p;
}


//load local data and translate them to network sequence
char* Package::dump (char* data, size_t size) const
{
    if (size < (sizeof(_command) + sizeof(_verify)))
        return NULL;

    char* p = data;

    *((uint16_t*)data) = htons(_command);
    p += sizeof(uint16_t);

    memcpy(p, _verify, sizeof(_verify));
    p += sizeof(_verify);

    return p;
}

void Package::show_debug()
{
    fprintf(stderr, "command = %d, verify = %s\n", _command, _verify ? _verify : "NULL");
}

const char* LicensePackage::load(const char* data)
{
    const char* d_data = Package::load(data);
    return d_data;
}

char* LicensePackage::dump (char* data, size_t size) const
{
    if (size < (sizeof(_command) + sizeof(_verify)))
        return NULL;

    char* p = Package::dump(data,size);

    *((uint16_t*)data) = htons(_command);
    p += sizeof(uint16_t);

    memcpy(p, _verify, sizeof(_verify));
    p += sizeof(_verify);

    return p;
}

const char* PrnlistPackage::load(const char* data)
{
    const char* d_data = Package::load(data);
    m_printerlist = d_data;
    return d_data;
}

char* PrnlistPackage::dump (char* data, size_t size) const
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

const char* FilePackage::load(const char* data)
{
    const char* d_data = Package::load(data);
    return d_data;
}

char* FilePackage::dump (char* data, size_t size) const
{
    if (size < (sizeof(_command) + sizeof(_verify)))
        return NULL;

    char* p = Package::dump(data,size);
    size -= (p-data);
    *((qint32*)p) = htonl(copies);
    p += sizeof(qint32);

    *((qint32*)p) = htonl(devUriSize);
    p += sizeof(qint32);
    memcpy(p,devUri.data(),devUriSize);
    p += devUriSize;

    *((qint32*)p) = htonl(fileNameSize);
    p += sizeof(qint32);
    memcpy(p,fileName.data(),fileNameSize);

    p += fileNameSize;
    *((qint32*)p) = htonl(fileSize);
    p += sizeof(qint32);
    memcpy(p,fileData.data(),fileSize);
    p += fileSize;

    return p;
}
