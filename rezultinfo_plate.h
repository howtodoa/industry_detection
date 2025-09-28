#ifndef REZULTINFO_PLATE_H
#define REZULTINFO_PLATE_H

#include <QObject>
#include "rezultinfo.h"
#include "CapacitanceProgram.h"
#include "typdef.h"
#include "public.h"


class RezultInfo_Plate : public RezultInfo
{
    Q_OBJECT
public:
    explicit RezultInfo_Plate(Parameters* params, QObject *parent = nullptr);

    void updatePaintData(Parameters *params) override;

    void printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange) override;

    int judge_plate(const  OutPlateResParam& ret) override;

  
};

#endif // REZULTINFO_PLATE_H
