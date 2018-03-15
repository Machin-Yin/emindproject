#include "printerthread.h"
#include "packagecreate.h"
#include "package.h"
#include "package.h"
#include <windows.h>
#include <Tlhelp32.h>
#include <printerdialog.h>
#include <QPrinterInfo>


PrinterThread::PrinterThread(QObject* parent, qintptr socket) : QThread(parent), _socket(socket)
{
}

void PrinterThread::run()
{
    qDebug()<<__FUNCTION__<<endl;
    QTcpSocket* socket = new QTcpSocket();
    socket->setSocketDescriptor(_socket);

    char buf[4];
    char* p = buf;
    socket->waitForReadyRead();
    socket->read(p, sizeof(buf));
    qint32 size = ntohl(*((qint32*)(void*)p));
    qDebug()<<"size="<<size<<endl;

    while (socket->bytesAvailable() < size) {
        if (!socket->waitForReadyRead()) {
            qDebug() << socket->errorString()<< endl;
            delete socket;
            return;
        }
        qDebug() <<"bytesAvailable()=" << socket->bytesAvailable() <<endl;
    }

    QByteArray data = socket->readAll();
    qDebug()<<"data.length()="<<data.length() << "size = " << size <<endl;


    Package* package = create_package_from_data(data.data());
    if (package){
        package->show_debug();
        switch (package->_command) {
        case VERIFY_PACKAGE:
        {
            if (package->_verify != savedAuthCode)
            {
                qDebug() << "package->_verify != savedAuthCode" ;
                sndMsg(VERIFY_PACKAGE,'0',socket);
            }
            else
            {
                qDebug() << "package->_verify == authcode" <<endl;
                sndMsg(VERIFY_PACKAGE,'1',socket);
            }
            break;
        }

        case PRINTER_LIST:
        {

            QString prStr = getPrlist();
            PackPrinter prpackage;
            prpackage._command = PRINTER_LIST;
            prpackage.m_printerlist = prStr;
            if (package->_verify != savedAuthCode)
            {
                qDebug() << "package->_verify != savedAuthCode" <<endl;
                *prpackage._verify = '0';
            }
            else
            {
                *prpackage._verify = '1';
            }
            char buffer[1024];
            char* p = buffer + sizeof(qint32);
            p = prpackage.dump(p, sizeof(buffer) - (p - buffer));
            char* org = buffer;
            *((qint32*)org) = htons((qint32)(p - buffer - sizeof(qint32)));
            socket->write(buffer, p - buffer);
            socket->flush();
            qDebug() << "send printer list over!" << endl;

            break;

        }
        case PRINT_FILE:
        {

            if (package->_verify != savedAuthCode )
            {
                qDebug() << "package->_verify != savedAuthCode" <<endl;
                qDebug() << "The authcode has changed!" <<endl;

                sndMsg(PRINT_FILE,'0',socket);

                if ((QFile::remove(my_tempFile) == 1))
                {
                    qDebug() << "delete" << my_tempFile << "success!" << endl;
                }
            }
            else
            {
                sndMsg(PRINT_FILE,'1',socket);
                setDefPrinter(socket);
            }

            break;
        }
        default:
            break;
        }
    }else{
        qDebug()<<"Package is null"<<endl;
    }
    socket->close();
    qDebug()<<"socket closed"<<endl;
    delete socket;
}

QString PrinterThread::getPrlist()
{
    QPrinterInfo pInfo;
    QStringList pname;
    pname = pInfo.availablePrinterNames();
    qDebug() << "panme:" << pname;
    foreach(auto a, pname)
    {
        qDebug() << a;
    }
    QString pstr = pname.join(",");
    return pstr;
}

void PrinterThread::setDefPrinter(QTcpSocket *psocket)
{
    qDebug() << __FUNCTION__ << endl;

    LPCWSTR printerName = (const wchar_t*)my_prName.utf16();
    SetDefaultPrinter(printerName);

    /****** Set printer property! ******/
    LPDEVMODE lpDevMode = NULL;
    HANDLE hPrinter;
    DWORD dwNeeded, dwRet;

    TCHAR defPrinter[256] = { 0 };
    memset(defPrinter, 0, 256);
    DWORD lengthDefpr = 256;
    GetDefaultPrinter(defPrinter, &lengthDefpr);
    qDebug() << "defPrinter===" << defPrinter << endl;
    //	LPDEVMODE defdevmode = getDefaultPdevmode(hPrinter);
    qDebug() << "defPrinter===" << defPrinter << endl;
    if (!OpenPrinter(defPrinter, &hPrinter, NULL))
    {
        qDebug() << "OpenPrinter==" << !OpenPrinter(defPrinter, &hPrinter, NULL) << endl;
        return;
    }
    //get real size of DEVMODE
    dwNeeded = DocumentProperties(NULL, hPrinter, defPrinter, NULL, NULL, 0);
    lpDevMode = (LPDEVMODE)malloc(dwNeeded);
    dwRet = DocumentProperties(NULL, hPrinter, defPrinter, lpDevMode, NULL, DM_OUT_BUFFER);
    qDebug() << "dwRet==" << dwRet << endl;
    if (dwRet != IDOK)
    {
        free(lpDevMode);
        ClosePrinter(hPrinter);
        return;
    }
    if (lpDevMode->dmFields & DM_COPIES)
    {
        lpDevMode->dmCopies = my_copies;
        lpDevMode->dmFields |= DM_COPIES;
    }
    if (lpDevMode->dmFields & DM_ORIENTATION)
    {
        /* If the printer supports paper orientation, set it.*/
        lpDevMode->dmOrientation = DMORIENT_LANDSCAPE;
        lpDevMode->dmOrientation |= DM_ORIENTATION;
    }
    //	if (lpDevMode->dmFields & DM_PAPERSIZE)
    //	{
    //		lpDevMode->dmPaperSize = DMPAPER_SIZE;
    //		lpDevMode->dmOrientation |= DM_PAPERSIZE;
    //	}
    dwRet = DocumentProperties(NULL, hPrinter, defPrinter, lpDevMode, lpDevMode, DM_IN_BUFFER | DM_OUT_BUFFER);
    //ClosePrinter(hPrinter);
    if (dwRet != IDOK)
    {
        free(lpDevMode);
        return;
    }

    //HDC hdc = CreateDC( (LPCWSTR)(_T("winspool").AllocSysString(), printerName , NULL, lpDevMode);
    DWORD dw;
    PRINTER_INFO_2 *pi2;
    GetPrinter(hPrinter, 2, NULL, 0, &dw);
    pi2 = (PRINTER_INFO_2*)GlobalAllocPtr(GHND, dw);
    GetPrinter(hPrinter, 2, (LPBYTE)pi2, dw, &dw);

    qDebug() << "pi2->pDevMode before" << pi2->pDevMode << endl;
    qDebug() << "lpDevMode before" << lpDevMode << endl;

    pi2->pDevMode = lpDevMode;
    SetPrinter(hPrinter, 2, (LPBYTE)pi2, 0);

    doPrint(psocket);

    ClosePrinter(hPrinter);
    GlobalFreePtr(pi2);

    //	SetDefaultPrinter(szBufferDefaultPrinterName);
    //	setDefaultPdevmode(hPrinter, defdevmode);   ////

}


void PrinterThread::doPrint(QTcpSocket *psocket)
{
    qDebug() << "my_tempFile==" <<my_tempFile<<endl;
    int ret = 0;
    ret = (int)ShellExecute(NULL,
                            QString("open").toStdWString().c_str(),
                            QString("SumatraPDF.exe").toStdWString().c_str(),
                            QString("-print-to-default %1 ").arg(my_tempFile).toStdWString().c_str(),
                            NULL,
                            SW_HIDE);

    if(ret>32)
    {
        sndMsg(PRINT_FILE,'O',psocket);
    }
    else
    {
        sndMsg(PRINT_FILE,'F',psocket);
        qDebug() << "PrintFailure" ;
    }

    qDebug() << "ret====" << ret << endl;

    QString rmvFile = my_tempFile;
    _sleep(200 * 1000);
    if ((QFile::remove(rmvFile) == 1))
    {
        qDebug() << "delete" << rmvFile << "success!" << endl;
    }

}

void PrinterThread::sndMsg(qint16 cmd,const char vry,QTcpSocket *psocket)
{
    Package package;
    package._command = cmd;
    *package._verify = vry;
    char buffer[1024];
    char* p = buffer + sizeof(qint32);
    p = package.dump(p, sizeof(buffer) - (p - buffer));
    char* org = buffer;
    *((qint32*)org) = htons((qint32)(p - buffer - sizeof(qint32)));
    psocket->write(buffer, p - buffer);
    psocket->flush();
}




