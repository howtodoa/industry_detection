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
    float SJ_WidthMin = 6.0f;
    float SJ_WidthMax = 20.0f;
    float JM_Height = 7.0f;
    int JM_Thresholdup = 56;
    int JM_Thresholddown = 29;

    // 瑕疵检测
    float AX_max = 10.0f;
    float CMAX_max = 10.0f;
    float HS_max = 10.0f;
    float ZW_max = 10.0f;

private slots:
};
