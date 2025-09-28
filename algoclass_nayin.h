#ifndef ALGOCLASS_NAYIN_H
#define ALGOCLASS_NAYIN_H

#include "algoclass.h"
#include <QObject>


class AlgoClass_NaYin : public AlgoClass
{
    Q_OBJECT
public:
    explicit AlgoClass_NaYin(QObject *parent = nullptr);
    explicit AlgoClass_NaYin(QString algopath, int al_core_param, float* Angle, QObject* parent = nullptr);

    virtual QWidget* createLeftPanel(QWidget* parent) override;
    virtual void saveParamAsync() override;

private:
    int al_core;
    float *Angle;


private slots:
    void onGetStampMaskExpect();
    void onGetStampSegmentImg();
    void onGetStampDetectImg();
    void onGetStampCropimage();
    void onGetStampCropSegmentImg();
    void onRunStamp();
    void onResultOutStamp();

   
};



#endif // ALGOCLASS_NAYIN_H
