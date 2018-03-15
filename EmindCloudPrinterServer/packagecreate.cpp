#include "packagecreate.h"
#include <QTcpSocket>
#include <string.h>
#include <windows.h>
//#include <arpa/inet.h>
#include <qdebug.h>
Package* create_package_from_data(const char* data)
{
    qDebug()<<"data="<<data<<endl;
    if (NULL == data)
        return NULL;
    quint16 command = ntohs(*((quint16*)data));

    switch (command) {
    case VERIFY_PACKAGE:
    {
        Package* package = new Package();
        package->load(data);
        return package;
    }
        break;
    case PRINTER_LIST:
    {
        Package* package = new PackPrinter();
        package->load(data);
        return package;
    }
    case PRINT_FILE:
    {
        Package* package = new PackFile();
        package->load(data);
        return package;
    }

    default:
        return NULL;
    }
}


