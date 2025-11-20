#pragma once

#include <QObject>
#include "rezultinfo.h"

class RezultInfo_Ny : public RezultInfo
{
    Q_OBJECT
public:
    // 1. 构造函数
    explicit RezultInfo_Ny(AllUnifyParams& unifyParams, QObject* parent = nullptr);

    // 2. 打印信息函数 (保持 override)
    void printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange) override;

    // 3. 判定函数 (参数改为 NYResult)
    virtual int judge_ny(const NYResult& ret) override;

};