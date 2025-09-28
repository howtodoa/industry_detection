#ifndef ALGOCLASS_LIFT_H
#define ALGOCLASS_LIFT_H

#include "algoclass.h"
#include "CapacitanceProgram.h"

class AlgoClass_Lift : public AlgoClass
{
public:
    explicit AlgoClass_Lift(QObject *parent = nullptr);
    explicit AlgoClass_Lift(QString algopath, int al_core_param, float* Angle, QObject* parent = nullptr);

    virtual QWidget* createLeftPanel(QWidget* parent) override;
private:
    int al_core;
    float* Angle;

private slots:
    void onGetLiftDetect1Img();
    void onResultOutLift();
    void onRunLift();

};

#endif // ALGOCLASS_LIFT_H
