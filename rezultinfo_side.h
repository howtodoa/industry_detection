#ifndef REZULTINFO_SIDE_H
#define REZULTINFO_SIDE_H

#include <QObject>
#include "rezultinfo.h"

class RezultInfo_Side : public RezultInfo
{
    Q_OBJECT
public:
    explicit RezultInfo_Side(Parameters* rangepara, QObject* parent);

    void printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange) override;

    int judge_side(const OutSideParam& ret) override;

    void updatePaintData(Parameters* rangePara) override;

    void  updateSidePaintVector(Parameters* rangePara);
};

#endif // REZULTINFO_SIDE_H
