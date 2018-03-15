#ifndef MAINAPPLICATION_H
#define MAINAPPLICATION_H

#include <QObject>
#include <qtsingleapplication/qtsingleapplication.h>
#include "logger.h"
namespace EPT {


class MainApplication : public SharedTools::QtSingleApplication
{
public:
    MainApplication(int &argc,char **argv);
    ~MainApplication();
signals:
    void runtimeWarn();

protected:
    void setupLogging();
    void setupTranslations();

private:
    QString _logFile;
    QString _logDir;
    int     _logExpire;
    bool    _logFlush;

};


}
#endif // MAINAPPLICATION_H
