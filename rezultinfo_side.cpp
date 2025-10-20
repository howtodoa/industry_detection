#include "rezultinfo_side.h"
#include <QRandomGenerator>
#include <sstream>

RezultInfo_Side::RezultInfo_Side(Parameters* rangepara, QObject* parent)
    : RezultInfo(rangepara, parent) // 基类构造函数
{
    initSidePaintVector(rangepara); // 可复用 Lift 的初始化逻辑，若有独立 side 的函数，也可以替换
}

void RezultInfo_Side::printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange)
{
    QString status = outOfRange ? "FAIL" : "PASS";
    QString limitType = isUpperLimit ? "Max" : "Min";

    qDebug() << status << ":" << paramName
        << "| Actual:" << actualValue
        << "|" << limitType << ":" << thresholdValue;
}

void RezultInfo_Side::updateSidePaintVector(Parameters* rangePara)
{
    if (!rangePara) {
        qWarning() << "updateSidePaintVector: 参数为空，无法更新。";
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
        if (item.label == "容积字符信息" || item.label == "电压字符信息") {
            // 这两项只显示，不判断，通常没有需要更新的 check 或 offset
            continue;
        }
        else if (item.label == "容积匹配") {
            if (paramMap.contains("m_VolumeOK")) item.check = paramMap["m_VolumeOK"].check;
        }
        else if (item.label == "电压匹配") {
            if (paramMap.contains("m_VoltageOK")) item.check = paramMap["m_VoltageOK"].check;
        }
        else if (item.label == "针脚状态") {
            if (paramMap.contains("m_PinOK")) item.check = paramMap["m_PinOK"].check;
        }
        else if (item.label == "成型脚距F") {
            bool hasLower = paramMap.contains("m_PinDisLower");
            bool hasUpper = paramMap.contains("m_PinDisUpper");
            item.check = (hasLower && paramMap["m_PinDisLower"].check) || (hasUpper && paramMap["m_PinDisUpper"].check);
            item.offset = hasLower ? paramMap["m_PinDisLower"].compensation : (hasUpper ? paramMap["m_PinDisUpper"].compensation : 0.0);
        }
        else if (item.label == "编带尺寸H") {
            bool hasLower = paramMap.contains("m_Case2TapeDisLower");
            bool hasUpper = paramMap.contains("m_Case2TapeDisUpper");
            item.check = (hasLower && paramMap["m_Case2TapeDisLower"].check) || (hasUpper && paramMap["m_Case2TapeDisUpper"].check);
            item.offset = hasLower ? paramMap["m_Case2TapeDisLower"].compensation : (hasUpper ? paramMap["m_Case2TapeDisUpper"].compensation : 0.0);
        }
        else if (item.label == "针脚宽度") {
            bool hasLower = paramMap.contains("m_PinWidthLower");
            bool hasUpper = paramMap.contains("m_PinWidthUpper");
            item.check = (hasLower && paramMap["m_PinWidthLower"].check) || (hasUpper && paramMap["m_PinWidthUpper"].check);
            item.offset = hasLower ? paramMap["m_PinWidthLower"].compensation : (hasUpper ? paramMap["m_PinWidthUpper"].compensation : 0.0);
        }
        else if (item.label == "套管状态") {
            if (paramMap.contains("m_CaseOK")) item.check = paramMap["m_CaseOK"].check;
        }
        else if (item.label == "套管高度") {
            bool hasLower = paramMap.contains("m_CaseHeighLower");
            bool hasUpper = paramMap.contains("m_CaseHeighUpper");
            item.check = (hasLower && paramMap["m_CaseHeighLower"].check) || (hasUpper && paramMap["m_CaseHeighUpper"].check);
            item.offset = hasLower ? paramMap["m_CaseHeighLower"].compensation : (hasUpper ? paramMap["m_CaseHeighUpper"].compensation : 0.0);
        }
        else if (item.label == "H-H0") {
            bool hasLower = paramMap.contains("m_Case2PinDisLower");
            bool hasUpper = paramMap.contains("m_Case2PinDisUpper");
            item.check = (hasLower && paramMap["m_Case2PinDisLower"].check) || (hasUpper && paramMap["m_Case2PinDisUpper"].check);
            item.offset = hasLower ? paramMap["m_Case2PinDisLower"].compensation : (hasUpper ? paramMap["m_Case2PinDisUpper"].compensation : 0.0);
        }
        else if (item.label == "套管Y差值") {
            bool hasLower = paramMap.contains("m_CaseYSiteLower");
            bool hasUpper = paramMap.contains("m_CaseYSiteUpper");
            item.check = (hasLower && paramMap["m_CaseYSiteLower"].check) || (hasUpper && paramMap["m_CaseYSiteUpper"].check);
            item.offset = hasLower ? paramMap["m_CaseYSiteLower"].compensation : (hasUpper ? paramMap["m_CaseYSiteUpper"].compensation : 0.0);
        }
        else if (item.label == "套管颜色") {
            if (paramMap.contains("m_CaseColor")) item.check = paramMap["m_CaseColor"].check;
        }
        else if (item.label == "胶带跑上跑下") {
            bool hasLower = paramMap.contains("m_TapeDisLower");
            bool hasUpper = paramMap.contains("m_TapeDisUpper");
            item.check = (hasLower && paramMap["m_TapeDisLower"].check) || (hasUpper && paramMap["m_TapeDisUpper"].check);
            item.offset = hasLower ? paramMap["m_TapeDisLower"].compensation : (hasUpper ? paramMap["m_TapeDisUpper"].compensation : 0.0);
        }
        else if (item.label == "破损检测") {
            if (paramMap.contains("m_BreakOK")) item.check = paramMap["m_BreakOK"].check;
        }
    }

    qDebug() << "m_PaintData(Side) 配置更新完成。";
}

void RezultInfo_Side::updatePaintData(Parameters* rangePara)
{
    updateSidePaintVector(rangePara);
}

int RezultInfo_Side::judge_side(const OutSideParam& ret)
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
            qWarning() << "judge_side - PaintData index out of range:" << currentPaintDataIndex << "/" << m_PaintData.size();
        }
        ++currentPaintDataIndex;
        };

    auto fillPaintDataItem = [&](const std::stringstream& ss, int result, double offset) {
        if (currentPaintDataIndex < m_PaintData.size()) {
            m_PaintData[currentPaintDataIndex].value = QString::fromStdString(ss.str());
            m_PaintData[currentPaintDataIndex].result = result;
            m_PaintData[currentPaintDataIndex].offset = offset;
        }
        else {
            qWarning() << "judge_side - PaintData index out of range:" << currentPaintDataIndex << "/" << m_PaintData.size();
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
                    shownValue = isgreater(shownValue, up) ? (up - (up - low) * random_num * 0.1) : (low + (up - low) * random_num * 0.1);
                }
            }
            else if (val > up && val <= (up + compUp)) {
                passed = true;
                offset = -compUp;
                shownValue = val - compUp;
                if (shownValue > up || shownValue < low) {
                    shownValue = isgreater(shownValue, up) ? (up - (up - low) * random_num * 0.1) : (low + (up - low) * random_num * 0.1);
                }
            }
        }
        else if (hasLow) {
            if (val >= low || val >= (low - compLow)) {
                passed = true;
                offset = (val < low) ? compLow : 0.0;
                shownValue = (val < low) ? val + compLow : val;
            }
        }
        else if (hasUp) {
            if (val <= up || val <= (up + compUp)) {
                passed = true;
                offset = (val > up) ? -compUp : 0.0;
                shownValue = (val > up) ? val - compUp : val;
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
            m_PaintData[currentPaintDataIndex].count++;
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
            m_PaintData[currentPaintDataIndex].count++;
        }

        printCheckInfo(name, actualStatus ? 1.0f : 0.0f, expectedFloat, false, !passed);
        updatePaintDataItem(actualStatus ? 1.0 : 0.0, passed ? 1 : 0, 0.0);
        hasAnyFail |= !passed;
        };

    auto fillString = [&](const QString& name, const std::stringstream& ss, const QString& keyExpected) {
        fillPaintDataItem(ss, 1, 0.0);
        };

    fillString("容积字符信息", ret.m_VolumeStr, "");
    fillString("电压字符信息", ret.m_VoltageStr, "");

    checkExistence("容积匹配", ret.m_VolumeOK, "m_VolumeOK");
    if (hasAnyFail) return -1;

    checkExistence("电压匹配", ret.m_VoltageOK, "m_VoltageOK");
    if (hasAnyFail) return -1;

    checkExistence("针脚状态", ret.m_PinOK, "m_PinOK");
    if (hasAnyFail) return -1;

    checkExistence("破损检测", ret.m_BreakOK, "m_BreakOK");
    if (hasAnyFail) return -1;

    checkExistence("套管颜色", ret.m_CaseColor, "m_CaseColor");
    if (hasAnyFail) return -1;

    checkExistence("套管状态", ret.m_CaseOK, "m_CaseOK");
    if (hasAnyFail) return -1;

    checkRange("成型脚距F", ret.m_PinDis, "m_PinDisLower", "m_PinDisUpper");
    if (hasAnyFail) return -1;

    checkRange("针脚宽度", ret.m_PinWidth, "m_PinWidthLower", "m_PinWidthUpper");
    if (hasAnyFail) return -1;

    checkRange("套管高度", ret.m_CaseHeigh, "m_CaseHeighLower", "m_CaseHeighUpper");
    if (hasAnyFail) return -1;

    checkRange("H-H0", ret.m_Case2PinDis, "m_Case2PinDisLower", "m_Case2PinDisUpper");
    if (hasAnyFail) return -1;

    checkRange("套管Y差值", ret.m_CaseYSite, "m_CaseYSiteLower", "m_CaseYSiteUpper");
    if (hasAnyFail) return -1;

    checkRange("胶带跑上跑下", ret.m_TapeDis, "m_TapeDisLower", "m_TapeDisUpper");
    if (hasAnyFail) return -1;

    checkRange("编带尺寸H", ret.m_Case2TapeDis, "m_Case2TapeDisLower", "m_Case2TapeDisUpper");
    if (hasAnyFail) return -1;


    if (currentPaintDataIndex != m_PaintData.size()) {
        qWarning() << "judge_side - PaintData size mismatch! Processed:" << currentPaintDataIndex << " Total:" << m_PaintData.size();
    }

    qDebug() << "All Side checks passed";
    return 0;
}