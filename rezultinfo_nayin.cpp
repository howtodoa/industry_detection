#include "rezultinfo_nayin.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>

RezultInfo_NaYin::RezultInfo_NaYin(QObject *parent)
    : RezultInfo(nullptr, parent)
{
}

bool RezultInfo_NaYin::parseRangeString(const QString& rangeStr, QVariant::Type expectedType, QVariant& minVal, QVariant& maxVal) const
{
    QStringList parts = rangeStr.split('-');
    if (parts.size() != 2) {
        return false;
    }

    bool okMin, okMax;
    QVariant parsedMin, parsedMax;

    if (expectedType == QVariant::Int) {
        parsedMin = parts.at(0).toInt(&okMin);
        parsedMax = parts.at(1).toInt(&okMax);
    }
    else if (expectedType == QVariant::Double) {
        parsedMin = parts.at(0).toFloat(&okMin);
        parsedMax = parts.at(1).toFloat(&okMax);
    } else {
        return false;
    }

    if (!okMin || !okMax) {
        return false;
    }

    minVal = parsedMin;
    maxVal = parsedMax;
    return true;
}

int RezultInfo_NaYin::judge(const OutResParam &ret)
{
    const QMap<QString, QMap<QString, ParamDetail>>& processedData = this->getProcessedData();

    auto checkValueInRange = [&](const QString& paramJsonName, const QVariant& actualValue) {
        for (const auto& projectEntry : processedData) {
            if (projectEntry.contains(paramJsonName)) {
                const ParamDetail& detail = projectEntry.value(paramJsonName);
                if (detail.check) {
                    QVariant minVal, maxVal;
                    if (parseRangeString(detail.range, actualValue.type(), minVal, maxVal)) {
                        bool outOfRange = false;
                        if (actualValue.type() == QVariant::Int) {
                            if (actualValue.toInt() < minVal.toInt() || actualValue.toInt() > maxVal.toInt()) {
                                outOfRange = true;
                            }
                        }
                        else if (actualValue.type() == QVariant::Double) {
                            if (actualValue.toFloat() < minVal.toFloat() || actualValue.toFloat() > maxVal.toFloat()) {
                                outOfRange = true;
                            }
                        }
                        if (outOfRange) {
                            return false;
                        }
                    }
                }
                return true;
            }
        }
        return true;
    };

    if (!checkValueInRange("正极宽度检测(正偏差)", QVariant(ret.posWidth))) { return -1; }
    if (!checkValueInRange("负极宽度检测(正偏差)", QVariant(ret.negWidth))) { return -1; }

    if (!checkValueInRange("正极瑕疵检测(总面积)", QVariant(ret.posErr))) { return -1; }
    if (!checkValueInRange("负极瑕疵检测(总面积)", QVariant(ret.negErr))) { return -1; }

    if (!checkValueInRange("负极标识检测", QVariant(ret.logoScores))) { return -1; }

    QVariant phiLowerBound, phiUpperBound;
    bool hasPhiLower = false;
    bool hasPhiUpper = false;

    // 声明一个临时的 QVariant 变量，用于接收不需要的值
    QVariant tempIgnoredValue;

    for (const auto& projectEntry : processedData) {
        if (projectEntry.contains("电容角度检测(下限)")) {
            const ParamDetail& detail = projectEntry.value("电容角度检测(下限)");
            if (detail.check) {
                // 将 QVariant() 替换为 tempIgnoredValue
                if (parseRangeString(detail.range, QVariant::Int, phiLowerBound, tempIgnoredValue)) {
                    hasPhiLower = true;
                }
            }
            break;
        }
    }

    for (const auto& projectEntry : processedData) {
        if (projectEntry.contains("电容角度检测(上限)")) {
            const ParamDetail& detail = projectEntry.value("电容角度检测(上限)");
            if (detail.check) {
                // 将 QVariant() 替换为 tempIgnoredValue
                if (parseRangeString(detail.range, QVariant::Int, tempIgnoredValue, phiUpperBound)) {
                    hasPhiUpper = true;
                }
            }
            break;
        }
    }

    if (hasPhiLower && hasPhiUpper) {
        if (ret.phi < phiLowerBound.toInt() || ret.phi > phiUpperBound.toInt()) {
            return -1;
        }
    }

    for (int i = 0; i < ret.textScores.size(); ++i) {
        QString charName = QString("字符%1").arg(i + 1);
        if (!checkValueInRange(charName, QVariant(ret.textScores[i]))) {
            return -1;
        }
    }

    return 0;
}
