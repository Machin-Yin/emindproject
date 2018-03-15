#include "packagecreate.h"
#include <QTcpSocket>
#include <string.h>
#include <arpa/inet.h>

//create package from received data
Package* create_package_from_data(const char* data)
{
    if (NULL == data)
        return NULL;
    uint16_t command = ntohs(*((uint16_t*)data));

    switch (command) {
    case VERIFY_PACKAGE:
    {
        Package* package = new LicensePackage();
        package->load(data);
        return package;
    }
        break;

    case PRINTER_LIST:
    {
        Package* package = new PrnlistPackage();
        package->load(data);
        return package;
    }
        break;
    case PRINT_FILE:
    {
        Package* package = new FilePackage();
        package->load(data);
        return package;
    }
        break;
    default:
        return NULL;
    }
}


/*                 TEST
void send_message_test(const char* message)
{
    QTcpSocket* client = new QTcpSocket();
    client->connectToHost("127.0.0.1", 1234);

    if (client->waitForConnected(1000)) {
        Package package;
        package._command = VERIFY_PACKAGE;
        memcpy(package._verify, message, sizeof(package._verify));

        char buffer[1024];
        char* p = buffer + sizeof(uint16_t);

        p = package.dump(p, sizeof(buffer) - (p - buffer));
        char* org = buffer;
        *((uint16_t*)org) = htons((uint16_t)(p - buffer - sizeof(uint16_t)));
        client->write(buffer, p - buffer);
        client->waitForBytesWritten(1000);
        client->disconnectFromHost();
    }
}
*/
