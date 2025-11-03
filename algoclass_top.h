#pragma once
#include "algoclass.h"
#include <QObject>

class AlgoClass_Top : public AlgoClass
{
    Q_OBJECT
public:
    explicit AlgoClass_Top(QObject* parent = nullptr);
    explicit AlgoClass_Top(QString algopath, int al_core, float* Angle, QObject* parent = nullptr);

    virtual QWidget* createLeftPanel(QWidget* parent) override;
    virtual void saveParamAsync() override;

private:
    int al_core;
    float* Angle;

private slots:

};
