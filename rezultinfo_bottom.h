#pragma once
#include <QObject>
#include "rezultinfo.h"
// 确保在此处或 rezultinfo.h 中包含了 Crop_BottomResult 的定义
// #include "DataStructs.h"

class RezultInfo_Bottom : public RezultInfo
{
    Q_OBJECT
public:
    // 1. 构造函数
    explicit RezultInfo_Bottom(AllUnifyParams& unifyParams, QObject* parent = nullptr);

    // 2. 打印信息函数 (保持 override)
    void printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange) override;

    // 3. 判定函数 (参数为 Crop_BottomResult)
    // 请确保基类 RezultInfo 中声明了 virtual int judge_bottom(const Crop_BottomResult& ret);
    virtual int judge_bottom(const Crop_BottomResult& ret) override;
};