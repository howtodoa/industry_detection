#ifndef OUTPUTTHREAD_FLOWER_H
#define OUTPUTTHREAD_FLOWER_H

#include "outputhread.h"   // 基类
#include <QDebug>
#include "typdef.h"
class OutPutThread_Flower : public OutPutThread
{
    Q_OBJECT

public:
    explicit OutPutThread_Flower(QObject* parent = nullptr);
    ~OutPutThread_Flower() override;

protected:
    void run() override;   // 线程执行体：花针专用逻辑
};

#endif // OUTPUTTHREAD_FLOWER_H
