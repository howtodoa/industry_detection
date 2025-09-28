#ifndef ALGOCLASS_PLATE_H
#define ALGOCLASS_PLATE_H

#include "algoclass.h"

class AlgoClass_Plate : public AlgoClass
{
public:
    explicit AlgoClass_Plate(QObject *parent = nullptr);
    explicit AlgoClass_Plate(QString algopath, int al_core, float* Angle, QObject* parent = nullptr);
    virtual QWidget* createLeftPanel(QWidget* parent) override;
private:
    int al_core;
    float* Angle;


private slots:
    void onGetDetectorPlateImg();
    void onRunPlate();
    void onGetSegmentPlateImg();
    void onResultOutPlate();
};

#endif // ALGOCLASS_PLATE_H
