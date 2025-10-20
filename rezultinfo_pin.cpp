#include "rezultinfo_pin.h"
#include <QRandomGenerator>
RezultInfo_Pin::RezultInfo_Pin(Parameters* rangepara, QObject* parent)
    : RezultInfo(rangepara, parent)
{
    initPinPaintVector(rangepara);
}

void RezultInfo_Pin::printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange)
{
    QString status = outOfRange ? "FAIL" : "PASS";
    QString limitType = isUpperLimit ? "Max" : "Min";

    qDebug() << status << ":" << paramName
        << "| Actual:" << actualValue
        << "|" << limitType << ":" << thresholdValue;
}

void RezultInfo_Pin::updatePaintData(Parameters* rangePara)
{
    updatePinPaintVector(rangePara);
}


void RezultInfo_Pin::updatePinPaintVector(Parameters* rangePara)
{
    if (!rangePara) {
        qWarning() << "updatePinPaintVector: 参数为空，无法更新。";
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

        // 根据 item.label (缺陷名称) 找到对应的配置并更新
        if (item.label == "孔毛刺") {
            bool hasLower = paramMap.contains("m_MaoCiAreaLower");
            bool hasUpper = paramMap.contains("m_MaoCiAreaUpper");
            item.check = (hasLower && paramMap["m_MaoCiAreaLower"].check) || (hasUpper && paramMap["m_MaoCiAreaUpper"].check);
            item.offset = hasLower ? paramMap["m_MaoCiAreaLower"].compensation : (hasUpper ? paramMap["m_MaoCiAreaUpper"].compensation : 0.0);
        }
        else if (item.label == "定位圆间距") {
            bool hasLower = paramMap.contains("m_DisCircleLower");
            bool hasUpper = paramMap.contains("m_DisCircleUpper");
            item.check = (hasLower && paramMap["m_DisCircleLower"].check) || (hasUpper && paramMap["m_DisCircleUpper"].check);
            item.offset = hasLower ? paramMap["m_DisCircleLower"].compensation : (hasUpper ? paramMap["m_DisCircleUpper"].compensation : 0.0);
        }
        else if (item.label == "成型脚距F") {
            bool hasLower = paramMap.contains("m_DisTopPinLower");
            bool hasUpper = paramMap.contains("m_DisTopPinUpper");
            item.check = (hasLower && paramMap["m_DisTopPinLower"].check) || (hasUpper && paramMap["m_DisTopPinUpper"].check);
            item.offset = hasLower ? paramMap["m_DisTopPinLower"].compensation : (hasUpper ? paramMap["m_DisTopPinUpper"].compensation : 0.0);
        }
        else if (item.label == "成型不良") {
            bool hasLower = paramMap.contains("m_DisButtomPinLower");
            bool hasUpper = paramMap.contains("m_DisButtomPinUpper");
            item.check = (hasLower && paramMap["m_DisButtomPinLower"].check) || (hasUpper && paramMap["m_DisButtomPinUpper"].check);
            item.offset = hasLower ? paramMap["m_DisButtomPinLower"].compensation : (hasUpper ? paramMap["m_DisButtomPinUpper"].compensation : 0.0);
        }
        else if (item.label == "针最大宽度") {
            bool hasLower = paramMap.contains("m_DisPinWidLower");
            bool hasUpper = paramMap.contains("m_DisPinWidUpper");
            item.check = (hasLower && paramMap["m_DisPinWidLower"].check) || (hasUpper && paramMap["m_DisPinWidUpper"].check);
            item.offset = hasLower ? paramMap["m_DisPinWidLower"].compensation : (hasUpper ? paramMap["m_DisPinWidUpper"].compensation : 0.0);
        }
        else if (item.label == "胶带破洞") {
            bool hasLower = paramMap.contains("m_totalAreaLower");
            bool hasUpper = paramMap.contains("m_totalAreaUpper");
            item.check = (hasLower && paramMap["m_totalAreaLower"].check) || (hasUpper && paramMap["m_totalAreaUpper"].check);
            item.offset = hasLower ? paramMap["m_totalAreaLower"].compensation : (hasUpper ? paramMap["m_totalAreaUpper"].compensation : 0.0);
        }
        else if (item.label == "粘胶不牢") {
            bool hasLower = paramMap.contains("m_DisTapeWidLower");
            bool hasUpper = paramMap.contains("m_DisTapeWidUpper");
            item.check = (hasLower && paramMap["m_DisTapeWidLower"].check) || (hasUpper && paramMap["m_DisTapeWidUpper"].check);
            item.offset = hasLower ? paramMap["m_DisTapeWidLower"].compensation : (hasUpper ? paramMap["m_DisTapeWidUpper"].compensation : 0.0);
        }
    }

    qDebug() << "m_PaintData(Pin) 配置更新完成。";
}

int RezultInfo_Pin::judge_pin(const OutPinParam& ret)
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
            qWarning() << "judge_pin - PaintData index out of range:" << currentPaintDataIndex << "/" << m_PaintData.size();
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
                if (shownValue > up || shownValue < low) {
                    shownValue = shownValue > up ? up - (up - low) * random_num * 0.1 : low + (up - low) * random_num * 0.1;
                }
            }
            else if (val > up && val <= (up + compUp)) {
                passed = true;
                offset = -compUp;
                shownValue = val - compUp;
                if (shownValue > up || shownValue < low) {
                    shownValue = shownValue > up ? up - (up - low) * random_num * 0.1 : low + (up - low) * random_num * 0.1;
                }
            }
        }
        double threshold = (!passed && val < low) ? low : (!passed && val > up) ? up : 0.0;
        bool isUpper = val > up;

        if (!passed && currentPaintDataIndex < m_PaintData.size()) {
            m_PaintData[currentPaintDataIndex].count++;
        }

        printCheckInfo(name, val, threshold, isUpper, !passed);
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
        bool expected = p ? (p->value.toFloat() == 1.0f) : false;
        bool passed = (actualStatus == expected);

        if (!passed && currentPaintDataIndex < m_PaintData.size()) {
            m_PaintData[currentPaintDataIndex].count++;
        }

        printCheckInfo(name, actualStatus ? 1.0f : 0.0f, expected ? 1.0f : 0.0f, false, !passed);
        updatePaintDataItem(actualStatus ? 1.0 : 0.0, passed ? 1 : 0, 0.0);
        hasAnyFail |= !passed;
        };

    checkRange("孔毛刺", ret.m_MaoCiArea, "m_MaoCiAreaLower","m_MaoCiAreaUpper");
    if (hasAnyFail) return -1;

    checkRange("定位圆间距", ret.m_DisCircle, "m_DisCircleLower", "m_DisCircleUpper");
    if (hasAnyFail) return -1;

    checkRange("成型脚距F", ret.m_DisTopPin, "m_DisTopPinLower", "m_DisTopPinUpper");
    if (hasAnyFail) return -1;

    checkRange("成型不良", ret.m_DisButtomPin, "m_DisButtomPinLower", "m_DisButtomPinUpper");
    if (hasAnyFail) return -1;

    checkRange("针最大宽度", ret.m_DisPinWid, "m_DisPinWidLower", "m_DisPinWidUpper");
    if (hasAnyFail) return -1;

    checkRange("中间胶带破洞", ret.m_totalArea, "m_totalAreaLower", "m_totalAreaUpper");
    if (hasAnyFail) return -1;

    checkRange("粘胶不牢", ret.m_DisTapeWid, "m_DisTapeWidLower", "m_DisTapeWidUpper");
    if (hasAnyFail) return -1;

    if (currentPaintDataIndex != m_PaintData.size()) {
        qWarning() << "judge_pin - PaintData size mismatch! Written:" << currentPaintDataIndex << " Total:" << m_PaintData.size();
    }

    qDebug() << "All Pin checks passed";
    return 0;
}