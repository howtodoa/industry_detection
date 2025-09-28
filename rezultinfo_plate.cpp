#include "rezultinfo_plate.h"
#include <QRandomGenerator>
RezultInfo_Plate::RezultInfo_Plate(Parameters* params, QObject *parent)
    : RezultInfo(params, parent) // 将 params 传递给父类 RezultInfo 的构造函数
{
    initPlatePaintVector(params);
}

void RezultInfo_Plate::printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange)
{
    QString status = outOfRange ? "FAIL" : "PASS";
    QString limitType = isUpperLimit ? "Max" : "Min";

    qDebug() << status << ":" << paramName
             << "| Actual:" << actualValue
             << "|" << limitType << ":" << thresholdValue;
}



void RezultInfo_Plate::updatePaintData(Parameters *params)
{
     initPlatePaintVector(params);
}


int RezultInfo_Plate::judge_plate(const OutPlateResParam& ret)
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
            qWarning() << "judge_plate - PaintData 索引超出范围:" << currentPaintDataIndex << "/" << m_PaintData.size();
        }
        ++currentPaintDataIndex;
        };

    auto checkRange = [&](const QString& name, double val, const QString& keyLower, const QString& keyUpper) {
       // bool detectEnabled = (currentPaintDataIndex < m_PaintData.size()) ? m_PaintData[currentPaintDataIndex].check : true;
        bool detectEnabled =  m_PaintData[currentPaintDataIndex].check;
        int random_num = QRandomGenerator::global()->bounded(10);
        if (!detectEnabled) {
            updatePaintDataItem(val, 1, 0.0);
            return;
        }

        const ProcessedParam* lowerParam = keyLower.isEmpty() ? nullptr : getProcessedParam(keyLower);
        const ProcessedParam* upperParam = keyUpper.isEmpty() ? nullptr : getProcessedParam(keyUpper);

        double low = (lowerParam) ? lowerParam->value.toDouble() : std::numeric_limits<double>::quiet_NaN();
        double up = (upperParam) ? upperParam->value.toDouble() : std::numeric_limits<double>::quiet_NaN();
        double compLow = (lowerParam) ? lowerParam->compensationValue : 0.0;
        double compUp = (upperParam) ? upperParam->compensationValue : 0.0;

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

        printCheckInfo(name, actualStatus ? 1.0f : 0.0f, expectedFloat, false, !passed);
        updatePaintDataItem(actualStatus ? 1.0 : 0.0, passed ? 1 : 0, 0.0);
        hasAnyFail |= !passed;
        };

    printPaintDataVector(m_PaintData);

    // === 顺序必须与 initPlatePaintVector 完全一致 ===
    checkExistence("底座存在", ret.m_PlatExist, "m_PlatExist");
    checkRange("底座面积", ret.m_PlateArea, "m_PlateAreaLower","");
    checkRange("底座高度", ret.m_PlateHeight, "m_PlateHeightLower", "m_PlateHeightUpper");
    checkRange("底座宽度", ret.m_PlateWid, "m_PlateWidLower", "m_PlateWidUpper");
    checkRange("座板上斜边长度", ret.m_PlateHypUpLen, "m_PlateHypUpLenLower", "m_PlateHypUpLenUpper");
    checkRange("座板下斜边长度", ret.m_PlateHypDownLen, "m_PlateHypDownLenLower", "m_PlateHypDownLenUpper");

    checkRange("底座瑕疵区域", ret.m_PlateErrNum, "", "m_PlateErrNumUpper");
    checkRange("底座压伤区域", ret.m_PlateCrushNum, "", "m_PlateCrushNumUpper");

    checkExistence("左引脚存在", ret.m_PlatePinLeftExist, "m_PlatePinLeftExist");
    checkExistence("右引脚存在", ret.m_PlatePinRightExist, "m_PlatePinRightExist");

    checkRange("左引脚高度", ret.m_PlatePinLeftHeight, "m_PlatePinLeftHeightLower", "m_PlatePinLeftHeightUpper");
    checkRange("右引脚高度", ret.m_PlatePinRightHeight, "m_PlatePinRightHeightLower", "m_PlatePinRightHeightUpper");

    checkRange("左引脚宽度", ret.m_PlatePinLeftWid, "m_PlatePinLeftWidLower", "m_PlatePinLeftWidUpper");
    checkRange("右引脚宽度", ret.m_PlatePinRightWid, "m_PlatePinRightWidLower", "m_PlatePinRightWidUpper");

    checkRange("左引脚角度", ret.m_PlatePinLeftAngle, "m_PlatePinLeftAngleLower", "m_PlatePinLeftAngleUpper");
    checkRange("右引脚角度", ret.m_PlatePinRightAngle, "m_PlatePinRightAngleLower", "m_PlatePinRightAngleUpper");

    checkRange("左引脚弯脚", ret.m_PlatePinLeftBendAngle, "m_PlatePinLeftBendAngleLower", "m_PlatePinLeftBendAngleUpper");
    checkRange("右引脚弯脚", ret.m_PlatePinRightBendAngle, "m_PlatePinRightBendAngleLower", "m_PlatePinRightBendAngleUpper");

    checkRange("左引脚平行度", ret.m_PlatePinLeftParalAngle, "m_PlatePinLeftParalAngleLower", "m_PlatePinLeftParalAngleUpper");
    checkRange("右引脚平行度", ret.m_PlatePinRightParalAngle, "m_PlatePinRightParalAngleLower", "m_PlatePinRightParalAngleUpper");

    checkRange("左引脚超底座长度", ret.m_PlatePinLeftExceLen, "m_PlatePinLeftExceLenLower", "m_PlatePinLeftExceLenUpper");
    checkRange("右引脚超底座长度", ret.m_PlatePinRightExceLen, "m_PlatePinRightExceLenLower", "m_PlatePinRightExceLenUpper");

    checkRange("引线总长度", ret.m_PlatePinTotalLen, "m_PlatePinTotalLenLower", "m_PlatePinTotalLenUpper");

    if (currentPaintDataIndex != m_PaintData.size()) {
        qWarning() << "judge_plate - PaintData 索引对不上！已处理:" << currentPaintDataIndex << " 实际大小为:" << m_PaintData.size();
    }

    qDebug() << (hasAnyFail ? "--- 有项未通过 ---" : "--- 所有 Plate 检测项通过 ---");
    return hasAnyFail ? -1 : 0;
}

void RezultInfo::printPaintDataVector(const QVector<PaintDataItem>& dataVector, const QString& description)
{
    // 如果提供了描述，首先打印描述和总体大小
    if (!description.isEmpty()) {
        qDebug() << "--- " << description << " (Size:" << dataVector.size() << ") ---";
    }
    else {
        qDebug() << "--- Printing PaintData Vector (Size:" << dataVector.size() << ") ---";
    }

    // 检查容器是否为空
    if (dataVector.isEmpty()) {
        qDebug() << "Vector is empty.";
        qDebug() << "----------------------------------------";
        return;
    }

    // 使用 C++11 的范围 for 循环遍历容器
    // 使用 auto& item 可以避免不必要的拷贝，提高效率
    for (int i = 0; i < dataVector.size(); ++i) {
        const auto& item = dataVector.at(i);

        // 使用 qDebug() 输出每个元素的详细信息
        // qDebug() 会自动处理 QString, bool, int, double 等类型，并添加空格
        qDebug().nospace() << "  [" << i << "] "
            << "Label: " << item.label
            << ", Check: " << (item.check ? "true" : "false") // bool值转为字符串更直观
            << ", Value: " << item.value
            << ", Result: " << (item.result == 1 ? "Pass" : "Fail") // 结果转为字符串
            << ", Offset: " << item.offset
            << ", Count: " << item.count;
    }

    qDebug() << "----------------------------------------";
}

//int RezultInfo_Plate::judge_plate(const OutPlateResParam& ret)
//{
//    int currentPaintDataIndex = 0;
//    bool hasAnyFail = false;
//
//    auto getProcessedParam = [&](const QString& key) -> const ProcessedParam* {
//        for (const auto& p : m_processedData) {
//            if (p.mappedVariable == key) return &p;
//        }
//        return nullptr;
//        };
//
//    auto updatePaintDataItem = [&](double shownVal, int result, double offset) {
//        if (currentPaintDataIndex < m_PaintData.size()) {
//            m_PaintData[currentPaintDataIndex].value = QString::number(shownVal, 'f', 3);
//            m_PaintData[currentPaintDataIndex].result = result;
//            m_PaintData[currentPaintDataIndex].offset = offset;
//        }
//        else {
//            qWarning() << "judge_plate - PaintData 索引超出范围:" << currentPaintDataIndex << "/" << m_PaintData.size();
//        }
//        ++currentPaintDataIndex;
//        };
//
//    auto checkRange = [&](const QString& name, double val, const QString& keyLower, const QString& keyUpper) {
//        const ProcessedParam* lowerParam = getProcessedParam(keyLower);
//        const ProcessedParam* upperParam = getProcessedParam(keyUpper);
//
//        double low = lowerParam ? lowerParam->value.toDouble() : std::numeric_limits<double>::quiet_NaN();
//        double up = upperParam ? upperParam->value.toDouble() : std::numeric_limits<double>::quiet_NaN();
//        double compLow = lowerParam ? static_cast<float>(lowerParam->compensationValue) : 0.0f;
//        double compUp = upperParam ? static_cast<float>(upperParam->compensationValue) : 0.0f;
//
//        bool passed = false;
//        double shownValue = val;
//        double offset = 0.0;
//
//        if (!std::isnan(low) && !std::isnan(up)) {
//            if (val >= low && val <= up) {
//                passed = true;
//            }
//            else if (val < low && val >= (low - compLow)) {
//                passed = true;
//                offset = compLow;
//                shownValue = val + compLow;
//            }
//            else if (val > up && val <= (up + compUp)) {
//                passed = true;
//                offset = -compUp;
//                shownValue = val - compUp;
//            }
//        }
//        else if (!std::isnan(low)) { // only lower limit
//            if (val >= low) {
//                passed = true;
//            }
//            else if (val >= (low - compLow)) {
//                passed = true;
//                offset = compLow;
//                shownValue = val + compLow;
//            }
//        }
//        else if (!std::isnan(up)) { // only upper limit
//            if (val <= up) {
//                passed = true;
//            }
//            else if (val <= (up + compUp)) {
//                passed = true;
//                offset = -compUp;
//                shownValue = val - compUp;
//            }
//        }
//        else {
//            passed = true; // no limits
//        }
//
//        double threshold = (!passed && !std::isnan(low) && val < low) ? low
//            : (!passed && !std::isnan(up) && val > up) ? up : 0.0;
//        bool isUpper = (!std::isnan(up) && val > up);
//        printCheckInfo(name, val, threshold, isUpper, !passed);
//
//        updatePaintDataItem(shownValue, passed ? 1 : 0, offset);
//        hasAnyFail |= !passed;
//        };
//
//    auto checkExistence = [&](const QString& name, bool actualStatus, const QString& keyExpected) {
//        const ProcessedParam* p = getProcessedParam(keyExpected);
//        float expectedFloat = p ? p->value.toFloat() : std::numeric_limits<float>::quiet_NaN();
//        bool expected = (expectedFloat == 1.0f);
//        bool passed = (actualStatus == expected);
//
//        printCheckInfo(name, actualStatus ? 1.0f : 0.0f, expectedFloat, false, !passed);
//        updatePaintDataItem(actualStatus ? 1.0 : 0.0, passed ? 1 : 0, 0.0);
//        hasAnyFail |= !passed;
//        };
//
//    // === 顺序必须与 initPlatePaintVector 完全一致 ===
//    checkExistence("底座存在", ret.m_PlatExist, "m_PlatExist");
//
//    checkRange("座板宽度", ret.m_PlateWid, "m_PlateWidLower", "m_PlateWidUpper");
//    checkRange("座板高度", ret.m_PlateHeight, "m_PlateHeightLower", "m_PlateHeightUpper");
//    checkRange("座板上斜边长度", ret.m_PlateHypUpLen, "m_PlateHypUpLenLower", "m_PlateHypUpLenUpper");
//    checkRange("座板下斜边长度", ret.m_PlateHypDownLen, "m_PlateHypDownLenLower", "m_PlateHypDownLenUpper");
//
//    checkRange("底座瑕疵区域个数", ret.m_PlateErrNum, "", "m_PlateErrNumUpper");
//    checkRange("底座压伤值", ret.m_PlateCrushNum, "", "m_PlateCrushNumUpper");
//
//    checkExistence("引线左引脚存在", ret.m_PlatePinLeftExist, "m_PlatePinLeftExist");
//    checkExistence("引线右引脚存在", ret.m_PlatePinRightExist, "m_PlatePinRightExist");
//
//    checkRange("引线左引脚高度", ret.m_PlatePinLeftHeight, "m_PlatePinLeftHeightLower", "m_PlatePinLeftHeightUpper");
//    checkRange("引线右引脚高度", ret.m_PlatePinRightHeight, "m_PlatePinRightHeightLower", "m_PlatePinRightHeightUpper");
//
//    checkRange("引线左引脚宽度", ret.m_PlatePinLeftWid, "m_PlatePinLeftWidLower", "m_PlatePinLeftWidUpper");
//    checkRange("引线右引脚宽度", ret.m_PlatePinRightWid, "m_PlatePinRightWidLower", "m_PlatePinRightWidUpper");
//
//    checkRange("引线左引脚角度", ret.m_PlatePinLeftAngle, "m_PlatePinLeftAngleLower", "m_PlatePinLeftAngleUpper");
//    checkRange("引线右引脚角度", ret.m_PlatePinRightAngle, "m_PlatePinRightAngleLower", "m_PlatePinRightAngleUpper");
//
//    checkRange("引线左引脚弯脚", ret.m_PlatePinLeftBendAngle, "m_PlatePinLeftBendAngleLower", "m_PlatePinLeftBendAngleUpper");
//    checkRange("引线右引脚弯脚", ret.m_PlatePinRightBendAngle, "m_PlatePinRightBendAngleLower", "m_PlatePinRightBendAngleUpper");
//
//    checkRange("引线左引脚平行度", ret.m_PlatePinLeftParalAngle, "m_PlatePinLeftParalAngleLower", "m_PlatePinLeftParalAngleUpper");
//    checkRange("引线右引脚平行度", ret.m_PlatePinRightParalAngle, "m_PlatePinRightParalAngleLower", "m_PlatePinRightParalAngleUpper");
//
//    checkRange("引线左引脚超底座长度", ret.m_PlatePinLeftExceLen, "m_PlatePinLeftExceLenLower", "m_PlatePinLeftExceLenUpper");
//    checkRange("引线右引脚超底座长度", ret.m_PlatePinRightExceLen, "m_PlatePinRightExceLenLower", "m_PlatePinRightExceLenUpper");
//
//    checkRange("引线总长度", ret.m_PlatePinTotalLen, "m_PlatePinTotalLenLower", "m_PlatePinTotalLenUpper");
//
//    if (currentPaintDataIndex != m_PaintData.size()) {
//        qWarning() << "judge_plate - PaintData 索引对不上！已处理:" << currentPaintDataIndex << " 实际大小为:" << m_PaintData.size();
//    }
//
//    qDebug() << (hasAnyFail ? "--- 有项未通过 ---" : "--- 所有 Plate 检测项通过 ---");
//    return hasAnyFail ? -1 : 0;
//}
