#include "printerdialog.h"
#include "ui_printerdialog.h"
#include <QDebug>
#include <QTimer>
#include <QNetworkInterface>
#include <qfile.h>
#include <QSettings>
#include <QDir>
//#include <QSysInfo>

#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <Windows.h>
#include <QString>
#include <QApplication>
#include <QRegExp>

#pragma comment(lib, "IPHLPAPI.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

QString savedAuthCode;

using namespace std;

PrinterDialog::PrinterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrinterDialog)
{
    qDebug() << __FUNCTION__ << endl;
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::Dialog;
    //        flags |= Qt::CustomizeWindowHint;
    flags |= Qt::WindowMinimizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

    QRegExp regExp("[A-Za-z0-9]{1,14}");
    ui->authleEdit->setValidator(new QRegExpValidator(regExp,this));

    setAutoStart(true);
    resetAuth();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(regetIP()));
    timer->start(200);

    /************  systray ****************/
    createActions();
    createTrayIcon();
    trayIcon->show();
    connect(trayIcon , SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconIsActived(QSystemTrayIcon::ActivationReason)));

    tcpPort = 6666;
    if (!printerServer.listen(QHostAddress::Any, tcpPort))
    {
        qDebug() << printerServer.errorString();
        close();
        return;
    }
}


PrinterDialog::~PrinterDialog()
{
    delete ui;
}

void PrinterDialog::regetIP()
{
    QLabel *ipLabel[8]={ui->ipLabel1,ui->ipLabel2,ui->ipLabel3,ui->ipLabel4,ui->ipLabel5,ui->ipLabel6,ui->ipLabel7,ui->ipLabel8};
    for(int j=0;j<8;j++)
    {
        ipLabel[j]->setText("");
    }
    getIP();
}

void PrinterDialog::getIP()
{
    QLabel *ipLabel[8]={ui->ipLabel1,ui->ipLabel2,ui->ipLabel3,ui->ipLabel4,ui->ipLabel5,ui->ipLabel6,ui->ipLabel7,ui->ipLabel8};

    int i = 0;

    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;

    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL) {
        printf("Error allocating memory needed to call GetAdaptersinfo\n");
        return;
    }
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            printf("Error allocating memory needed to call GetAdaptersinfo\n");
            return;
        }
    }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
        pAdapter = pAdapterInfo;
        while (pAdapter) {
            switch (pAdapter->Type) {
            case MIB_IF_TYPE_OTHER:
                printf("Other\n");
                break;
            case MIB_IF_TYPE_ETHERNET:
                //                printf("Ethernet\n");
                break;
            case MIB_IF_TYPE_TOKENRING:
                printf("Token Ring\n");
                break;
            case MIB_IF_TYPE_FDDI:
                printf("FDDI\n");
                break;
            case MIB_IF_TYPE_PPP:
                printf("PPP\n");
                break;
            case MIB_IF_TYPE_LOOPBACK:
                printf("Lookback\n");
                break;
            case MIB_IF_TYPE_SLIP:
                printf("Slip\n");
                break;
            default:
                //                printf("Unknown type %ld\n", pAdapter->Type);
                break;
            }
            string ipstr0 = "0.0.0.0";
            if(pAdapter->IpAddressList.IpAddress.String != ipstr0)
            {

                ipLabel[i]->setText(pAdapter->IpAddressList.IpAddress.String);
            }
            else
            {
                i--;
            }
            pAdapter = pAdapter->Next;
            i++;
            printf("\n");
        }
    }
    else
    {
        printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);
    }
    if (pAdapterInfo)
        FREE(pAdapterInfo);
}

void PrinterDialog::on_flushButton_clicked()
{    
    QString authcode = ui->authleEdit->text();
    if(authcode == "")
    {
        QMessageBox::information(0,QObject::tr("Information"),QObject::tr("The authcode can't null!"),QMessageBox::tr("Yes"));
        resetAuth();
    }
    else
    {
        saveAuth(authcode);
    }
}

void PrinterDialog::setAutoStart(bool is_auto_start)
{
    QString application_name = QApplication::applicationName();
    QSettings *settings = new QSettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    if(is_auto_start)
    {
        QString application_path = QApplication::applicationFilePath();
        QString register_path = application_path.replace('/', '\\') + " -hide";
        settings->setValue(application_name,register_path);
    }
    else
    {
        settings->remove(application_name);
    }
    delete settings;
}

void PrinterDialog::iconIsActived(QSystemTrayIcon::ActivationReason reason)
{
    qDebug()<<__FUNCTION__<<endl;
    switch(reason)
    {
    case QSystemTrayIcon::Trigger:
    {
        if(this->isMinimized()|| this->isHidden())
        {
            showNormal();
            QApplication::setActiveWindow(this);
            ui->authleEdit->setText(savedAuthCode);
            break;
        }
        else if(this->isVisible())
        {
            this->hide();
            break;
        }
    }
    case QSystemTrayIcon::MiddleClick:
    {
        if(this->isMinimized()|| this->isHidden())
        {
            showNormal();
            QApplication::setActiveWindow(this);
            ui->authleEdit->setText(savedAuthCode);
            break;
        }
        else if(this->isVisible())
        {
            this->hide();
            break;
        }
    }
    default:
        break;
    }
}

void PrinterDialog::closeEvent(QCloseEvent *event)
{

    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
    }
}

void PrinterDialog::setVisible(bool visible)
{
    restoreAction->setEnabled(isMaximized() || !visible);
    QDialog::setVisible(visible);
}

void PrinterDialog::createActions()
{
    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);
    connect(restoreAction, &QAction::triggered, this, &PrinterDialog::slotresetAuth);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void PrinterDialog::slotresetAuth()
{
    ui->authleEdit->setText(savedAuthCode);
}

void PrinterDialog::resetAuth()
{
    if(QSysInfo::windowsVersion() == QSysInfo::WV_XP)
    {
        QSettings setting("authcode.ini",QSettings::IniFormat);
        setting.beginGroup("authcode");
        savedAuthCode=setting.value("authcode").toString();
        setting.endGroup();;
    }
    else
    {
        QString tempCodeFolder = QDir::tempPath();
        QString authFile = tempCodeFolder + "/" + "authcode.ini";
        QString defCode = "Emind";
        QSettings settings(authFile,QSettings::IniFormat);
        settings.beginGroup("authcode");
        QString authCode=settings.value("authcode").toString();
        qDebug() << "authCode==" <<authCode;
        if(authCode == NULL )
        {
            settings.setValue("authcode",defCode);
            savedAuthCode = defCode ;
        }
        else
        {
            savedAuthCode = authCode;
        }
        settings.endGroup();
    }
    ui->authleEdit->setText(savedAuthCode);
}

void PrinterDialog::saveAuth(QString authCode)
{
    if(QSysInfo::windowsVersion() == QSysInfo::WV_XP)
    {
        setAuth("authcode.ini",authCode);
        mutex.lock();
        savedAuthCode = authCode;
        mutex.unlock();
    }
    else
    {
        QString tempCodeFolder = QDir::tempPath();
        QString authFile = tempCodeFolder + "/" + "authcode.ini";
        setAuth(authFile,authCode);
        mutex.lock();
        savedAuthCode = authCode;
        mutex.unlock();
    }
}

void PrinterDialog::setAuth(QString iniPath,QString authcode)
{
    QSettings settings(iniPath,QSettings::IniFormat);
    settings.beginGroup("authcode");
    settings.setValue("authcode",authcode);
    settings.endGroup();
}

void PrinterDialog::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/image/tray.png"));
    trayIcon->setToolTip(tr("EmindCloudPrinter"));

}


void PrinterDialog::showEvent(QShowEvent *event)
{
    ui->authleEdit->setText(savedAuthCode);
    event->ignore();
}


