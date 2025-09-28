#pragma once
#include "algoclass.h"
#include <QObject>


class AlgoClass_Pin : public AlgoClass
{
    Q_OBJECT
public:
    explicit AlgoClass_Pin(QObject* parent = nullptr);
    explicit AlgoClass_Pin(QString algopath, int al_core_param, float* Angle, QObject* parent = nullptr);

    virtual QWidget* createLeftPanel(QWidget* parent) override;
    virtual void saveParamAsync() override;

private:
    int al_core;
    float* Angle;


private slots:



};



