#pragma once

#include <QObject>
#include "rezultinfo.h"


class RezultInfo_Xs : public RezultInfo
{
    Q_OBJECT
public:
    // 1. 构造函数改名
    explicit RezultInfo_Xs(AllUnifyParams& unifyParams, QObject* parent = nullptr);

    // 2. 打印信息函数 (保持 override)
    void printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange) override;

    virtual int judge_xs(const XSResult& ret) override;

};