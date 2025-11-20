#ifndef REZULTINFO_LIFT_H
#define REZULTINFO_LIFT_H

#include <QObject>
#include "rezultinfo.h"

class RezultInfo_Lift : public RezultInfo
{
    Q_OBJECT
public:
    explicit RezultInfo_Lift(Parameters *rangepara, QObject *parent);
    explicit RezultInfo_Lift(AllUnifyParams& unifyParam, QObject* parent = nullptr);


    void printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange) override;

    int judge_lift(const OutLiftResParam &ret) override;

    void updatePaintData(Parameters* rangePara) override;

};

#endif // REZULTINFO_LIFT_H
