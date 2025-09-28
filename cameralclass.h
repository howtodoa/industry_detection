#ifndef CAMERALCLASS_H
#define CAMERALCLASS_H

#include <QObject>
#include <QString>
#include <QDebug>
#include "typdef.h"
#include  "fileoperator.h"
#include "MsvDeviceLib.h"


class CameralClass : public QObject
{
    Q_OBJECT
public:
    explicit CameralClass(QObject *parent = nullptr);
    explicit CameralClass(QString cameralpath, Mz_CameraConn::COperation* camOp,QObject *parent = nullptr);

    void init();
    void change();

    CamParamControll& getParameters()  { return m_param; }

    void saveParamAsync();


private:
    QString m_cameralPath;
    CamParamControll m_param;
    Mz_CameraConn::COperation* m_camOp = nullptr;
};

#endif // CAMERALCLASS_H
