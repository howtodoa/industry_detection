#pragma once
#include "algoclass.h"
#include "CapacitanceProgram.h"   // 包含 OutAbutResParam 的定义
#include <QObject>
#include <QString>

class AlgoClass_Abut : public AlgoClass
{
    Q_OBJECT

public:
    explicit AlgoClass_Abut(QObject* parent = nullptr);
    explicit AlgoClass_Abut(QString algopath, int al_core_param, float* Angle, QObject* parent = nullptr);

    // 创建左侧面板
    virtual QWidget* createLeftPanel(QWidget* parent) override;
    virtual void saveParamAsync() override;
private:
    int al_core;  // 算法核心编号
	float* Angle; // 指向角度数据的指针

private slots:
    void onGetThresholdImgAbut();   // 获取弯脚二值化图片
    void onResultOutAbut();         // 输出弯脚检测结果
    void onRunAbut();               // 运行弯脚算法
};
