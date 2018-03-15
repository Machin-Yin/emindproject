#ifndef PRINTERDIALOG_H
#define PRINTERDIALOG_H

#include "printerserver.h"
#include <QDialog>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QCloseEvent>
//#include <QPrinter>

extern QString savedAuthCode;

namespace Ui {
	class PrinterDialog;
}

class PrinterDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PrinterDialog(QWidget *parent = 0);
	~PrinterDialog();
    void setAutoStart(bool is_auto_start);
    void setAuth(QString iniPath,QString authCode);
    QMutex mutex;

private:
	Ui::PrinterDialog *ui;
	PrinterServer printerServer;
	qint16 tcpPort;
    void getIP();
    void resetAuth();
    void saveAuth(QString authCode);

/*************** systray *************/
    QAction *restoreAction;
    QAction *quitAction;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    void createTrayIcon();
    void createActions();
    void setVisible(bool visible);
    void setIcon();
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *);

private slots:
    void regetIP();
	void on_flushButton_clicked();
    void slotresetAuth();
    void iconIsActived(QSystemTrayIcon::ActivationReason reason);
//    void onFinishEdit();
};

#endif // PRINTERDIALOG_H
