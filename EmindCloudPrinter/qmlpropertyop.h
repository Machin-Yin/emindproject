#ifndef QMLPROPERTYOP_H
#define QMLPROPERTYOP_H

#include <QObject>
#include <QQuickItem>
#include <QSharedDataPointer>

namespace EPT {
class QmlPropertyOpData;



class QmlPropertyOp : public QObject
{
    Q_OBJECT
public:
    explicit QmlPropertyOp(QObject* qmlObject,QObject *parent = 0);
    QmlPropertyOp(const QmlPropertyOp &);
    QmlPropertyOp &operator=(const QmlPropertyOp &);
    ~QmlPropertyOp();

//    static QmlPropertyOp* instance(QObject* qmlObject)
//    {
//        static QmlPropertyOp* inst = 0;
//        if(!inst){
//            inst = new QmlPropertyOp(qmlObject);
//        }
//        return inst;
//    }
    void showCWin();
    void showPWin();
signals:

public slots:
    void showWin(quint8);


private:
    QSharedDataPointer<QmlPropertyOpData> data;

};


}
#endif // QMLPROPERTYOP_H
