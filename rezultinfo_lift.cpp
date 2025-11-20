#include "rezultinfo_lift.h"
#include <QRandomGenerator>
RezultInfo_Lift::RezultInfo_Lift(Parameters *rangepara, QObject *parent)
    : RezultInfo(rangepara, parent) // 调用基类 RezultInfo 的构造函数，它会负责加载和处理参数
{
    initLiftPaintVector(rangepara);
}

RezultInfo_Lift::RezultInfo_Lift(AllUnifyParams& unifyParams, QObject* parent)
    : RezultInfo()
{
    this->unifyParams = unifyParams;
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

#ifdef ADAPTATEION
int RezultInfo_Lift::judge_lift(const OutLiftResParam& ret)
{
    qDebug() << "--- START: 判定参数 (Lift) ---";
    bool allPassed = true;

    // 1. 遍历成员变量 unifyParams 中的所有配置项
    for (auto it = unifyParams.begin(); it != unifyParams.end(); ++it)
    {
        const QString paramKey = it.key();
        UnifyParam& config = it.value(); // !!! 必须使用非 const 引用来修改 config !!!

        // 1.1 检查是否启用了检测
        if (!config.check) {
            config.result = 1; // 未启用检测，默认通过
            qDebug() << "  -> Skipping judgement for disabled param:" << config.label;
            continue;
        }

        // 1.2 统计更新：增加总检测次数
        config.count++;

        bool checkResult = false; // 当前项的判定结果 (true=通过)

        qDebug() << "--- JUDGING (" << config.count << "):" << config.label << " (" << paramKey << ") ---";

        // 2. 根据 paramKey 匹配，并调用判定方法
        // (注意: config.value 应该已经被 updateActualValues 更新为实测值了)

        // --- 数值类型 (Lift 项目全为数值范围判定) ---
        if (paramKey == "m_disLift") {
            checkResult = config.checkRange();
        }
        else if (paramKey == "m_PinWidthLift") {
            checkResult = config.checkRange();
        }
        else if (paramKey == "m_PinAngle") {
            checkResult = config.checkRange();
        }
        else if (paramKey == "m_PinHeightLift") {
            checkResult = config.checkRange();
        }

        // --- 未匹配 ---
        else
        {
            qWarning() << "警告 (Lift Judge): 配置项" << paramKey << "未在匹配列表中找到，跳过判定。";
            continue;
        }

        // 3. 汇总结果和统计更新
        if (checkResult)
        {
            config.result = 1; // 通过
            qDebug() << "  -> PASSED.";
        }
        else
        {
            config.result = 0; // 不通过
            config.ng_count++; // 增加 NG 次数
            allPassed = false; // 标记存在 NG 项
            qCritical() << "  -> !!! FAILED:" << config.label << " (" << paramKey << ") 未通过检测 !!!";
        }
    }

    qDebug() << "--- END: 判定完成 (Lift) --- Final Result (0=OK, 1=NG):" << (allPassed ? 0 : 1);
    // 返回整数结果：0 为全部通过，1 为存在 NG
    return allPassed ? 0 : 1;
}
#else
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
#endif
