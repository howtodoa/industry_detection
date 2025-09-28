#ifndef REZULTINFO_NAYIN_H
#define REZULTINFO_NAYIN_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVariant>
#include <vector>
#include<cmath>
#include "rezultinfo.h"
#include <opencv2/opencv.hpp>


class RezultInfo_NaYin : public RezultInfo
{
    Q_OBJECT
public:
    explicit RezultInfo_NaYin(Parameters* params, QObject *parent = nullptr);

    // 修改 printCheckInfo 以接受 float 类型，阈值统一是 float
    void printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange) override;
    void updatePaintData(Parameters *params) override;
    int  judge_stamp(const  OutStampResParam &ret) override;

    int judge_stamp_min(const OutStampResParam& ret) override;
 
    OutStampResParam outstamp;


private:

};


#endif // REZULTINFO_NAYIN_H
