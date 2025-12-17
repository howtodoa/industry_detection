#pragma once
#include "algoclass.h"
#include <QObject>

class AlgoClass_Ny : public AlgoClass
{
    Q_OBJECT
public:
    explicit AlgoClass_Ny(QObject* parent = nullptr);
    explicit AlgoClass_Ny(QString algopath, QObject* parent = nullptr);

    virtual QWidget* createLeftPanel(QWidget* parent) override;
    virtual void saveParamAsync() override;

public:
    // NY 的参数
    float CQ_AreaMin = 20.0f;
    float GS_AreaMin = 20.0f;
    float HS_AreaMin = 20.0f;
    float QP_AreaMin = 20.0f;
    float YH_AreaMin = 20.0f;
    float ZW_AreaMin = 10.0f;

    // 色差参数
    float NY_SeCha_Ratio = 0.3f;

    // 相似度阈值控制参数
    float OCR_Control = 0.6f;
    int XT_ZF_ColorThreshold = 50; //颜色过滤参数
    float NY_BDparam = 5;

    //型替字符相似度控制参数
    float XT_Similarity_Threshold = 0.9;
    //型替字符HSV颜色范围
    int NY_H = 80;
    int NY_S = 150;

private slots:

};
