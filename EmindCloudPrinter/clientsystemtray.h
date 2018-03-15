#ifndef CLIENTSYSTEMTRAY_H
#define CLIENTSYSTEMTRAY_H

#include <QObject>

class QSystemTrayIcon;
class QMenu;
class QAction;

namespace EPT {

class ClientSystemTray : public QObject
{
    Q_OBJECT
public:
    explicit ClientSystemTray(QObject *parent = 0);
    ~ClientSystemTray();
    void createActions();
    void createTrayIcon();
signals:

public slots:


private:
    QSystemTrayIcon * trayIcon;
    QMenu *trayIconMenu;
    QAction* showAction;
    QAction* quitAction;
    QObject* m_parent;
};

}
#endif // CLIENTSYSTEMTRAY_H
