#ifndef REZULTINFO_NAYIN_H
#define REZULTINFO_NAYIN_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVariant>
#include <vector>
#include<cmath>
#include "rezultinfo.h"

struct OutResParam
{
    int phi;
    int posWidth;
    int posErr;
    int negWidth;
    int negErr;
    int textNum;
    std::vector<float> textScores;
    float logoScores;
};

class RezultInfo_NaYin : public RezultInfo
{
    Q_OBJECT
public:
    explicit RezultInfo_NaYin(RangeParameters* params, QObject *parent = nullptr);

    // 修改 printCheckInfo 以接受 float 类型，阈值统一是 float
    void printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange);

    int judge(const OutResParam &ret);

private:

};


#endif // REZULTINFO_NAYIN_H
