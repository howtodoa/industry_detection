#ifndef REZULTINFO_NAYIN_H
#define REZULTINFO_NAYIN_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVariant>
#include <vector>

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
    explicit RezultInfo_NaYin(QObject *parent = nullptr);
    int judge(const OutResParam &ret);

private:
    bool parseRangeString(const QString& rangeStr, QVariant::Type expectedType, QVariant& minVal, QVariant& maxVal) const;
};

#endif // REZULTINFO_NAYIN_H
