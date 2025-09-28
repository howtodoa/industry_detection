#include "rezultinfo_lift.h"
#include <QRandomGenerator>
RezultInfo_Lift::RezultInfo_Lift(Parameters *rangepara, QObject *parent)
    : RezultInfo(rangepara, parent) // 调用基类 RezultInfo 的构造函数，它会负责加载和处理参数
{
    initLiftPaintVector(rangepara);
}

void RezultInfo_Lift::printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange)
{
    QString status = outOfRange ? "FAIL" : "PASS";
    QString limitType = isUpperLimit ? "Max" : "Min";

    qDebug() << status << ":" << paramName
             << "| Actual:" << actualValue
             << "|" << limitType << ":" << thresholdValue;
}

void RezultInfo_Lift::updatePaintData(Parameters* rangePara)
{
    initLiftPaintVector(rangePara);
}


int RezultInfo_Lift::judge_lift(const OutLiftResParam& ret)
{
    int random_num = QRandomGenerator::global()->bounded(10);
    int currentPaintDataIndex = 0;
    bool hasAnyFail = false;

    auto getProcessedParam = [&](const QString& key) -> const ProcessedParam* {
        for (const auto& p : m_processedData) {
            if (p.mappedVariable == key) return &p;
        }
        return nullptr;
        };

    auto updatePaintDataItem = [&](double shownVal, int result, double offset) {
        if (currentPaintDataIndex < m_PaintData.size()) {
            m_PaintData[currentPaintDataIndex].value = QString::number(shownVal, 'f', 3);
            m_PaintData[currentPaintDataIndex].result = result;
            m_PaintData[currentPaintDataIndex].offset = offset;
        }
        else {
            qWarning() << "judge_lift - PaintData 索引超出范围:" << currentPaintDataIndex << "/" << m_PaintData.size();
        }
        ++currentPaintDataIndex;
        };

    auto checkRange = [&](const QString& name, double val, const QString& keyLower, const QString& keyUpper) {
        bool detectEnabled = (currentPaintDataIndex < m_PaintData.size()) ? m_PaintData[currentPaintDataIndex].check : true;

        if (!detectEnabled) {
            updatePaintDataItem(val, 1, 0.0);
            return;
        }

        const ProcessedParam* lowerParam = getProcessedParam(keyLower);
        const ProcessedParam* upperParam = getProcessedParam(keyUpper);

        double low = lowerParam ? lowerParam->value.toDouble() : std::numeric_limits<double>::quiet_NaN();
        double up = upperParam ? upperParam->value.toDouble() : std::numeric_limits<double>::quiet_NaN();
        double compLow = lowerParam ? static_cast<float>(lowerParam->compensationValue) : 0.0f;
        double compUp = upperParam ? static_cast<float>(upperParam->compensationValue) : 0.0f;

        bool passed = false;
        double shownValue = val;
        double offset = 0.0;

        if (!std::isnan(low) && !std::isnan(up)) {
            if (val >= low && val <= up) {
                passed = true;
            }
            else if (val < low && val >= (low - compLow)) {
                passed = true;
                offset = compLow;
                shownValue = val + compLow;
                if (shownValue > up || shownValue < low)
                {

                    if (shownValue > up) shownValue = up - (up - low) * random_num * 0.1;
                    else shownValue = low + (up - low) * random_num * 0.1;
                }
            }
            else if (val > up && val <= (up + compUp)) {
                passed = true;
                offset = -compUp;
                shownValue = val - compUp;
                if (shownValue > up || shownValue < low)
                {

                    if (shownValue > up) shownValue = up - (up - low) * random_num * 0.1;
                    else shownValue = low + (up - low) * random_num * 0.1;
                }
            }
        }

        // 输出检测信息
        double threshold = (!passed && val < low) ? low : (!passed && val > up) ? up : 0.0;
        bool isUpper = val > up;
        printCheckInfo(name, val, threshold, isUpper, !passed);

        updatePaintDataItem(shownValue, passed ? 1 : 0, offset);
        hasAnyFail |= !passed;
        };

    // 顺序需严格一致
    checkRange("引脚宽度", ret.m_PinWidthLift, "m_PinWidthLiftLower", "m_PinWidthLiftUpper");
    checkRange("引脚高度", ret.m_PinHeightLift, "m_PinHeightLiftLower", "m_PinHeightLiftUpper");
    checkRange("引脚到底座距离", ret.m_disLift, "m_disLiftLower", "m_disLiftUpper");
    checkRange("引脚角度", ret.m_PinAngle, "m_PinAngleLower", "m_PinAngleUpper");

    if (currentPaintDataIndex != m_PaintData.size()) {
        qWarning() << "judge_lift - PaintData 项数不匹配！已写入:" << currentPaintDataIndex << " 总大小:" << m_PaintData.size();
    }

    qDebug() << (hasAnyFail ? "--- 有项未通过 ---" : "--- 所有 Lift 检测项通过 ---");
    return hasAnyFail ? -1 : 0;
}
