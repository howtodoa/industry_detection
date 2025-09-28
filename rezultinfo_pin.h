#ifndef REZULTINFO_PIN_H
#define REZULTINFO_PIN_H

#include "rezultinfo.h"
#include "BraidedTape.h"

class RezultInfo_Pin : public RezultInfo
{
public:
    explicit RezultInfo_Pin(Parameters* params, QObject* parent = nullptr);

    void printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange) override;

    int judge_pin(const OutPinParam& ret) override;

    void updatePaintData(Parameters* rangePara) override;

    void updatePinPaintVector(Parameters* rangePara);
};

#endif // REZULTINFO_PIN_H
