#ifndef REZULTINFO_TOP_H
#define REZULTINFO_TOP_H

#include <QObject>
#include "rezultinfo.h"

class RezultInfo_Top : public RezultInfo
{
    Q_OBJECT
public:
    explicit RezultInfo_Top(Parameters* rangepara, QObject* parent);

    void printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange) override;

    int judge_top(const OutTopParam& ret) override;

    void updatePaintData(Parameters* rangePara) override;

    void updateTopPaintVector(Parameters* rangePara);
};

#endif // REZULTINFO_TOP_H
