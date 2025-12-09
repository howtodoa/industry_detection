#pragma once
#include "algoclass.h"
#include <QObject>

class AlgoClass_Xs : public AlgoClass
{
    Q_OBJECT
public:
    explicit AlgoClass_Xs(QObject* parent = nullptr);
    explicit AlgoClass_Xs(QString algopath, QObject* parent = nullptr);

    virtual QWidget* createLeftPanel(QWidget* parent) override;
    virtual void saveParamAsync() override;

public:
    // 基本参数
    float SJ_WidthMin = 6.0f;      // 塑胶最小宽度
    float SJ_WidthMax = 20.0f;     // 塑胶最大宽度
    float JM_Height = 7.0f;        // 胶帽高度
    int JM_Thresholdup = 56;       // 胶帽亮度上阈值
    int JM_Thresholddown = 29;     // 胶帽亮度下阈值

    // 瑕疵检测
    float AX_max = 10.0f;          // 凹陷最大面积
    float CMAX_max = 10.0f;        // 凹陷面积
    float HS_max = 10.0f;          // 划伤最大面积
    float ZW_max = 10.0f;          // 脏污最大面积

    // 新增参数
    float XS_BDparam = 7.0f;       //算法标定

private slots:
};
