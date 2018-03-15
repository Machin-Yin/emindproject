#ifndef PRINTERWATCHER_H
#define PRINTERWATCHER_H

#include <QObject>

class PrinterWatcher : public QObject
{
    Q_OBJECT
public:
    explicit PrinterWatcher(QObject *parent = 0);

signals:

public slots:
};

#endif // PRINTERWATCHER_H