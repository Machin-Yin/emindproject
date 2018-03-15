#include "clientsystemtray.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QApplication>
namespace EPT {


ClientSystemTray::ClientSystemTray(QObject *parent) : QObject(parent),m_parent(parent)
{
    createActions();
    createTrayIcon();

}

ClientSystemTray::~ClientSystemTray()
{

}

void ClientSystemTray::createActions()
{
    showAction = new QAction(QIcon(":/img/print16.png"),tr("Show MainWindow"),m_parent);
    connect(showAction,SIGNAL(triggered(bool)),m_parent,SLOT(show()));
    quitAction = new QAction(QIcon(":/img/print16.png"),tr("Quit"),m_parent);
    connect(quitAction,SIGNAL(triggered(bool)),qApp,SLOT(quit()));
}


void ClientSystemTray::createTrayIcon()
{
    trayIcon = new QSystemTrayIcon(m_parent);
    trayIcon->setToolTip(tr("EmindCloudPrinter"));
    trayIcon->setIcon(QIcon(":/img/print16.png"));
    trayIconMenu = new QMenu();
    trayIconMenu->addAction(showAction);
    trayIconMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
}


}
