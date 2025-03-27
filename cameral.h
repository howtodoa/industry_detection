#ifndef CAMERAL_H
#define CAMERAL_H

#include <QObject>
#include "public.h"

class Cameral : public QObject
{
    Q_OBJECT
public:
    explicit Cameral(QObject *parent = nullptr);
    ~Cameral();

    // 三个结构体的成员变量
    RangePara rangeParams;       // 范围参数
    CameralPara cameralParams;   // 相机参数
    AlgorithmPara algoParams;    // 算法参数

signals:

public slots:

};

#endif // CAMERAL_H
