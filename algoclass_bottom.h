#pragma once
#include "algoclass.h"
#include <QObject>

class AlgoClass_Bottom : public AlgoClass
{
    Q_OBJECT
public:
    explicit AlgoClass_Bottom(QObject* parent = nullptr);
    explicit AlgoClass_Bottom(QString algopath, QObject* parent = nullptr);

    virtual QWidget* createLeftPanel(QWidget* parent) override;
    virtual void saveParamAsync() override;

public:
    float LK_min = 1.0f;
    float JM_min = 1.0f;
    float WB_min = 1.0f;

private slots:

};
