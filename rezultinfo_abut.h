#ifndef REZULTINFO_ABUT_H
#define REZULTINFO_ABUT_H

#include <QObject>
#include "rezultinfo.h"

class RezultInfo_Abut : public RezultInfo
{
    Q_OBJECT
public:
     explicit RezultInfo_Abut(Parameters *rangepara, QObject *parent = nullptr);

    void printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange) override;

     virtual int judge_abut(const OutAbutResParam &ret) override;

};

#endif // REZULTINFO_ABUT_H
