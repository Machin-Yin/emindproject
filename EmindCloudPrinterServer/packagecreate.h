#ifndef PACKAGE_CREATE_H
#define PACKAGE_CREATE_H

#include <QObject>
#include "package.h"

enum {
    VERIFY_PACKAGE = 1,
    LICENSE,
    PRINTER_LIST,
    PRINT_FILE
};


Package* create_package_from_data(const char* data);

#endif // PACKAGE_CREATE_H

