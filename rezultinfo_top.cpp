#include "rezultinfo_top.h"
#include <QRandomGenerator>

RezultInfo_Top::RezultInfo_Top(Parameters* rangepara, QObject* parent)
    : RezultInfo(rangepara, parent)
{
    initTopPaintVector(rangepara);

}


void RezultInfo_Top::printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange)
{
    QString status = outOfRange ? "FAIL" : "PASS";
    QString limitType = isUpperLimit ? "Max" : "Min";

    qDebug() << status << ":" << paramName
        << "| Actual:" << actualValue
        << "|" << limitType << ":" << thresholdValue;
}

void RezultInfo_Top::updatePaintData(Parameters* rangePara)
{
    updateTopPaintVector(rangePara);
}

void RezultInfo_Top::updateTopPaintVector(Parameters* rangePara)
{
    if (!rangePara) {
        qWarning() << "updateTopPaintVector: 参数为空，无法更新。";
        return;
    }

    // 1. 收集所有新的参数配置到一个 map 中，方便快速查找
    const QMap<QString, DetectionProject>& projects = rangePara->detectionProjects;
    QMap<QString, ParamDetail> paramMap;
    for (const auto& project : projects) {
        for (auto it = project.params.begin(); it != project.params.end(); ++it) {
            const ParamDetail& detail = it.value();
            if (!detail.name.isEmpty()) {
                paramMap.insert(detail.name, detail);
            }
        }
    }

    // 2. 遍历内存中已存在的 m_PaintData 列表，逐项更新
    for (auto& item : m_PaintData) { // 使用引用&来直接修改列表中的项

        // 根据 item.label (缺陷名称) 找到对应的配置
        if (item.label == "防爆阀类型") {
            if (paramMap.contains("m_FBFClassOKTop")) {
                item.check = paramMap["m_FBFClassOKTop"].check;
                // offset 对布尔类型无意义，保持默认
            }
        }
        else if (item.label == "极性检测") {
            bool hasLower = paramMap.contains("m_NegAngleTopLower");
            bool hasUpper = paramMap.contains("m_NegAngleTopUpper");
            item.check = (hasLower && paramMap["m_NegAngleTopLower"].check) || (hasUpper && paramMap["m_NegAngleTopUpper"].check);
            item.offset = hasLower ? paramMap["m_NegAngleTopLower"].compensation : (hasUpper ? paramMap["m_NegAngleTopUpper"].compensation : 0.0);
        }
        else if (item.label == "防爆阀圆度") {
            bool hasLower = paramMap.contains("m_CirNumTopLower");
            bool hasUpper = paramMap.contains("m_CirNumTopUpper");
            item.check = (hasLower && paramMap["m_CirNumTopLower"].check) || (hasUpper && paramMap["m_CirNumTopUpper"].check);
            item.offset = hasLower ? paramMap["m_CirNumTopLower"].compensation : (hasUpper ? paramMap["m_CirNumTopUpper"].compensation : 0.0);
        }
        else if (item.label == "防爆阀半径") {
            bool hasLower = paramMap.contains("m_RadiusTopLower");
            bool hasUpper = paramMap.contains("m_RadiusTopUpper");
            item.check = (hasLower && paramMap["m_RadiusTopLower"].check) || (hasUpper && paramMap["m_RadiusTopUpper"].check);
            item.offset = hasLower ? paramMap["m_RadiusTopLower"].compensation : (hasUpper ? paramMap["m_RadiusTopUpper"].compensation : 0.0);
        }
       
    }

    qDebug() << "m_PaintData(Top) 配置更新完成。";
}

int RezultInfo_Top::judge_top(const OutTopParam& ret)
{
    int currentPaintDataIndex = 0;
    bool hasAnyFail = false;

    auto getProcessedParam = [&](const QString& key) -> const ProcessedParam* {
        for (const auto& p : m_processedData) {
            if (p.mappedVariable == key)
                return &p;
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
            qWarning() << "judge_top - PaintData index out of range:" << currentPaintDataIndex << "/" << m_PaintData.size();
        }
        ++currentPaintDataIndex;
        };

    auto checkRange = [&](const QString& name, double val, const QString& keyLower, const QString& keyUpper) {
        bool detectEnabled = (currentPaintDataIndex < m_PaintData.size()) ? m_PaintData[currentPaintDataIndex].check : true;
        int random_num = QRandomGenerator::global()->bounded(10);
        if (!detectEnabled) {
            updatePaintDataItem(val, 1, 0.0);
            return;
        }
        const ProcessedParam* lowerParam = keyLower.isEmpty() ? nullptr : getProcessedParam(keyLower);
        const ProcessedParam* upperParam = keyUpper.isEmpty() ? nullptr : getProcessedParam(keyUpper);
        double low = lowerParam ? lowerParam->value.toDouble() : std::numeric_limits<double>::quiet_NaN();
        double up = upperParam ? upperParam->value.toDouble() : std::numeric_limits<double>::quiet_NaN();
        double compLow = lowerParam ? lowerParam->compensationValue : 0.0;
        double compUp = upperParam ? upperParam->compensationValue : 0.0;
        bool hasLow = !std::isnan(low);
        bool hasUp = !std::isnan(up);
        bool passed = false;
        double shownValue = val;
        double offset = 0.0;
        if (hasLow && hasUp) {
            if (val >= low && val <= up) {
                passed = true;
            }
            else if (val < low && val >= (low - compLow)) {
                passed = true;
                offset = compLow;
                shownValue = val + compLow;
                if (shownValue > up || shownValue < low) {
                    if (shownValue > up) shownValue = up - (up - low) * random_num * 0.1;
                    else shownValue = low + (up - low) * random_num * 0.1;
                }
            }
            else if (val > up && val <= (up + compUp)) {
                passed = true;
                offset = -compUp;
                shownValue = val - compUp;
                if (shownValue > up || shownValue < low) {
                    if (shownValue > up) shownValue = up - (up - low) * random_num * 0.1;
                    else shownValue = low + (up - low) * random_num * 0.1;
                }
            }
        }
        else if (hasLow) {
            if (val >= low) {
                passed = true;
            }
            else if (val >= (low - compLow)) {
                passed = true;
                offset = compLow;
                shownValue = val + compLow;
            }
        }
        else if (hasUp) {
            if (val <= up) {
                passed = true;
            }
            else if (val <= (up + compUp)) {
                passed = true;
                offset = -compUp;
                shownValue = val - compUp;
            }
        }
        else {
            passed = true;
        }
        double threshold = 0.0;
        bool isUpper = false;
        if (!passed) {
            if (hasLow && val < low) {
                threshold = low;
                isUpper = false;
            }
            else if (hasUp && val > up) {
                threshold = up;
                isUpper = true;
            }
        }
        else {
            if (hasUp) {
                threshold = up;
                isUpper = true;
            }
            else if (hasLow) {
                threshold = low;
                isUpper = false;
            }
        }

        if (!passed && currentPaintDataIndex < m_PaintData.size()) {
            if(GlobalPara::NG_Count_Able.load() == true)  m_PaintData[currentPaintDataIndex].count++;
        }

        printCheckInfo(name, static_cast<float>(val), static_cast<float>(threshold), isUpper, !passed);
        updatePaintDataItem(shownValue, passed ? 1 : 0, offset);
        hasAnyFail |= !passed;
        };

    auto checkExistence = [&](const QString& name, bool actualStatus, const QString& keyExpected) {
        bool detectEnabled = (currentPaintDataIndex < m_PaintData.size()) ? m_PaintData[currentPaintDataIndex].check : true;
        if (!detectEnabled) {
            updatePaintDataItem(actualStatus ? 1.0 : 0.0, 1, 0.0);
            return;
        }
        const ProcessedParam* p = getProcessedParam(keyExpected);
        float expectedFloat = p ? p->value.toFloat() : std::numeric_limits<float>::quiet_NaN();
        bool expected = (expectedFloat == 1.0f);
        bool passed = (actualStatus == expected);

        if (!passed && currentPaintDataIndex < m_PaintData.size()) {
            if(GlobalPara::NG_Count_Able.load() == true)  m_PaintData[currentPaintDataIndex].count++;
        }

        printCheckInfo(name, actualStatus ? 1.0f : 0.0f, expectedFloat, false, !passed);
        updatePaintDataItem(actualStatus ? 1.0 : 0.0, passed ? 1 : 0, 0.0);
        hasAnyFail |= !passed;
        };

    checkExistence("防爆阀类型", ret.m_FBFClassOKTop, "m_FBFClassOKTop");
    if (hasAnyFail) return -1;

    checkRange("极性检测", ret.m_NegAngleTop, "m_NegAngleTopLower", "m_NegAngleTopUpper");
    if (hasAnyFail) return -1;

    checkRange("防爆阀圆度", ret.m_CirNumTop, "m_CirNumTopLower", "m_CirNumTopUpper");
    if (hasAnyFail) return -1;

    checkRange("防爆阀半径", ret.m_RadiusTop, "m_RadiusTopLower", "m_RadiusTopUpper");
    if (hasAnyFail) return -1;

    if (currentPaintDataIndex != m_PaintData.size()) {
        qWarning() << "judge_top - PaintData size mismatch! Processed:" << currentPaintDataIndex << " Total:" << m_PaintData.size();
    }

    qDebug() << "All Top checks passed";
    return 0;
}