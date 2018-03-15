#include "qmlpropertyop.h"
#include "client.h"

namespace EPT {


class QmlPropertyOpData : public QSharedData
{
public:
    QObject* m_qmlObject1;
    QObject* m_qmlObject2;

};

QmlPropertyOp::QmlPropertyOp(QObject* qmlObject,QObject *parent) : QObject(parent), data(new QmlPropertyOpData())
{
    if(qmlObject){
        if(qmlObject->objectName() == "PrinterWin"){
             data->m_qmlObject2 = qmlObject;
             data->m_qmlObject2->setProperty("visible",false);
        }
        else if(qmlObject->objectName() == "ClientWin"){
            data->m_qmlObject1 = qmlObject;
//            data->m_qmlObject2->setProperty("visible",false);
       }

    }

    connect(client,SIGNAL(sigShowWin(quint8)),this,SLOT(showWin(quint8)));
}

QmlPropertyOp::QmlPropertyOp(const QmlPropertyOp &rhs) : data(rhs.data)
{

}

QmlPropertyOp &QmlPropertyOp::operator=(const QmlPropertyOp &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QmlPropertyOp::~QmlPropertyOp()
{

}

void QmlPropertyOp::showWin(quint8 winNo)
{
    qDebug()<<__FUNCTION__<<endl;
    if(winNo == 1)
        showCWin();
    else
        showPWin();
}

void QmlPropertyOp::showCWin()
{

    data->m_qmlObject1->setProperty("visible",true);
}

void QmlPropertyOp::showPWin()
{
    data->m_qmlObject2->setProperty("visible",true);
}

}
