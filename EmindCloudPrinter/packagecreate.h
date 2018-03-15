#ifndef PACKAGE_CREATE_H
#define PACKAGE_CREATE_H

#include <QObject>
#include "package.h"

enum MSG_TYPE{
	VERIFY_PACKAGE = 1,
    LICENSE,
    PRINTER_LIST,
    PRINT_FILE
};

Package* create_package_from_data(const char* data);



/*                 TEST                  */
void send_message_test(const char* message);


#endif // PACKAGE_CREATE_H
