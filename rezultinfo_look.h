#include <QObject>
#include "rezultinfo.h"

class RezultInfo_Look : public RezultInfo
{
    Q_OBJECT
public:
    explicit RezultInfo_Look(AllUnifyParams& unifyParams, QObject* parent = nullptr);

    void printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange) override;

    virtual int judge_look(const OutLookPinResParam& ret) override;

};