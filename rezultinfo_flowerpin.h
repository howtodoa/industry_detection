#include <QObject>
#include "rezultinfo.h"

class RezultInfo_FlowerPin : public RezultInfo
{
    Q_OBJECT
public:
    explicit RezultInfo_FlowerPin(AllUnifyParams& unifyParams, QObject* parent = nullptr);

    void printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange) override;

    virtual int judge_abut(const OutAbutResParam& ret) override;

};