#include "rezultinfo_nayin.h"
#include <QStringList>
#include <QRandomGenerator>
RezultInfo_NaYin::RezultInfo_NaYin(Parameters* params, QObject *parent)
    : RezultInfo(params, parent) // 将 params 传递给父类 RezultInfo 的构造函数
{
    initStampPaintVector(params);
}


RezultInfo_NaYin::RezultInfo_NaYin(AllUnifyParams& unifyParams, QObject* parent)
    : RezultInfo()
{
    this->unifyParams = unifyParams;
}


void RezultInfo_NaYin::printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange)
{
    QString status = outOfRange ? "FAIL" : "PASS";
    QString limitType = isUpperLimit ? "Max" : "Min";

    qDebug() << status << ":" << paramName
             << "| Actual:" << actualValue
             << "|" << limitType << ":" << thresholdValue;
}



void RezultInfo_NaYin::updatePaintData(Parameters *params)
{
    initStampPaintVector(params);
}

#ifdef ADAPTATEION

int RezultInfo_NaYin::judge_stamp(const OutStampResParam& ret)
{
    qDebug() << "--- START: 判定参数 (Stamp) ---";
    bool allPassed = true;

    // 1. [全局检查] 
    //    首先检查 "实测字符数" 是否等于 "学习字符数"
    //    这是独立于 map 遍历的第一个判定点。
    qDebug() << "--- JUDGING (Global): 字符总数 ---";
    if (ret.textNum != LearnPara::inNum)
    {
        qCritical() << "  -> !!! FAILED: 字符总数不匹配! 期望 (LearnPara::inNum):" << LearnPara::inNum
            << ", 实测 (ret.textNum):" << ret.textNum << " !!!";
        allPassed = false; // 标记存在 NG 项
        // 注意: 我们不在此处 return, 而是继续检查其他项
    }
    else
    {
        qDebug() << "  -> PASSED: 字符总数: " << ret.textNum;
    }


    // 2. [遍历检查] 遍历 unifyParams 中的所有配置项
    for (auto it = unifyParams.begin(); it != unifyParams.end(); ++it)
    {
        const QString paramKey = it.key();
        UnifyParam& config = it.value(); // !!! 必须使用非 const 引用来修改 config !!!

        // 2.1 检查是否启用了检测
        if (!config.check) {
            config.result = 1; // 未启用检测，默认通过
            qDebug() << "  -> Skipping judgement for disabled param:" << config.label;
            continue;
        }

        // 2.2 统计更新：增加总检测次数
        config.count++;

        bool checkResult = false; // 当前项的判定结果 (true=通过)

        qDebug() << "--- JUDGING (" << config.count << "):" << config.label << " (" << paramKey << ") ---";

        // 3. 根据 paramKey 匹配，并调用判定方法

        // --- 动态字符类型 (优先处理) ---
        if (paramKey.startsWith("character"))
        {
            bool ok = false;
            int characterIndex = paramKey.mid(9).toInt(&ok); // "character" 长度为 9

            // 3.1 检查键名是否合法
            if (!ok || characterIndex <= 0) {
                qWarning() << "警告 (Stamp Judge): 无效的 character 键:" << paramKey;
                continue;
            }

            // 3.2 检查此项是否在 "学习范围" 内
            // (如果 LearnPara::inNum=10, 则 "character11" 不应参与判定)
            if (characterIndex > LearnPara::inNum) {
                qDebug() << "  -> Skipping judgement for" << paramKey
                    << "(Index" << characterIndex << " > Learned textNum " << LearnPara::inNum << ")";
                config.result = 1; // 标记为通过 (N/A)
                continue;          // 跳过后续的汇总逻辑
            }

            // 3.3 检查此项是否在 "实测范围" 内
            // (在学习范围内 (<=10), 但实测只返回了 9 个)
            int vectorIndex = characterIndex - 1; // "character1" -> 索引 0
            if (vectorIndex >= ret.textScores.size())
            {
                qCritical() << "  -> !!! FAILED:" << config.label << " (" << paramKey
                    << ") - 期望检测 (Index " << characterIndex
                    << "), 但未在实测结果中找到 (Vector size " << ret.textScores.size() << ") !!!";
                checkResult = false; // 判定失败!
            }
            else
            {
                // 3.4 检查分数
                // (期望检测，且在实测中找到了。config.value 已被 updateActualValues 更新)
                qDebug() << "  -> Checking 字符得分...";
                checkResult = config.checkRange();
            }
        }

        // --- 其他数值类型 ---
        else if (paramKey == "capacitor_angle") {
            qDebug() << "  -> Checking 电容角度...";
            checkResult = config.checkRange();
        }
        else if (paramKey == "positive_width") {
            qDebug() << "  -> Checking 正极宽度...";
            checkResult = config.checkRange();
        }
        else if (paramKey == "negative_width") {
            qDebug() << "  -> Checking 负极宽度...";
            checkResult = config.checkRange();
        }
        else if (paramKey == "positive_defect_area") {
            qDebug() << "  -> Checking 正极瑕疵...";
            checkResult = config.checkRange();
        }
        else if (paramKey == "negative_mark") {
            qDebug() << "  -> Checking 负极标识...";
            checkResult = config.checkRange();
        }
        else if (paramKey == "negative_defect_area") {
            qDebug() << "  -> Checking 负极瑕疵...";
            checkResult = config.checkRange();
        }

        // --- 未匹配 ---
        else
        {
            qWarning() << "警告 (Stamp Judge): 配置项" << paramKey << "未在匹配列表中找到，跳过判定。";
            continue;
        }

        // 4. 汇总结果和统计更新
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
    } // --- End of for loop ---

    qDebug() << "--- END: 判定完成 (Stamp) --- Final Result (0=OK, 1=NG):" << (allPassed ? 0 : 1);

    // 返回整数结果：0 为全部通过，1 为存在 NG
    return allPassed ? 0 : 1;
}

#else

int RezultInfo_NaYin::judge_stamp(const OutStampResParam& ret)
{
    bool hasAnyFail = false;
    int currentPaintDataIndex = 0;

    auto updatePaintDataItem = [&](float value, int result) {
        if (currentPaintDataIndex < m_PaintData.size()) {
            m_PaintData[currentPaintDataIndex].value = QString::number(value, 'f', 3);
            m_PaintData[currentPaintDataIndex].result = result;
        }
        else {
            qWarning() << "judge_stamp - 索引超出范围: " << currentPaintDataIndex << "/" << m_PaintData.size();
        }
        };

    auto setPaintDataItemNoCheck = [&]() {
        if (currentPaintDataIndex < m_PaintData.size()) {
            m_PaintData[currentPaintDataIndex].value = "N/A";
            m_PaintData[currentPaintDataIndex].result = 2; // 不检测状态
        }
        else {
            qWarning() << "judge_stamp - 不检测处理，索引超出范围: " << currentPaintDataIndex << "/" << m_PaintData.size();
        }
        ++currentPaintDataIndex;
        };

    auto getProcessedParam = [&](const QString& key) -> const ProcessedParam* {
        for (const auto& p : m_processedData) {
            if (p.mappedVariable == key) return &p;
        }
        return nullptr;
        };

    auto getThresholdValue = [&](const QString& key) -> float {
        if (auto p = getProcessedParam(key)) return p->value.toFloat();
        return std::numeric_limits<float>::quiet_NaN();
        };

    // 双阈值范围检查（角度、宽度等）
    auto checkRange = [&](const QString& name, float value, const QString& keyLower, const QString& keyUpper) {
        if (currentPaintDataIndex >= m_PaintData.size() || !m_PaintData[currentPaintDataIndex].check) {
            setPaintDataItemNoCheck();
            return;
        }

        float lower = getThresholdValue(keyLower);
        float upper = getThresholdValue(keyUpper);

        float compLow = 0.0f;
        float compUp = 0.0f;
        if (auto pLow = getProcessedParam(keyLower)) compLow = pLow->compensationValue;
        if (auto pUp = getProcessedParam(keyUpper)) compUp = pUp->compensationValue;

        float adjLower = lower - compLow; // 下限补偿放宽
        float adjUpper = upper + compUp;  // 上限补偿放宽

        bool passed = true;
        if (!std::isnan(lower) && value < adjLower) passed = false;
        if (!std::isnan(upper) && value > adjUpper) passed = false;

        printCheckInfo(name, value, passed ? 0 : (value < adjLower ? adjLower : adjUpper), (value > adjUpper), !passed);
        hasAnyFail |= !passed;
        updatePaintDataItem(value, passed ? 1 : 0);
        ++currentPaintDataIndex;
        };

    // 仅上限检查（瑕疵检测，越小越好）
    auto checkUpperOnly = [&](const QString& name, float value, const QString& keyUpper) {
        if (currentPaintDataIndex >= m_PaintData.size() || !m_PaintData[currentPaintDataIndex].check) {
            setPaintDataItemNoCheck();
            return;
        }

        float upper = getThresholdValue(keyUpper);
        float comp = 0.0f;
        if (auto p = getProcessedParam(keyUpper)) comp = p->compensationValue;

        float adjUpper = upper + comp;  // 上限补偿放宽
        bool passed = std::isnan(upper) || value <= adjUpper;

        printCheckInfo(name, value, adjUpper, true, !passed);
        hasAnyFail |= !passed;
        updatePaintDataItem(value, passed ? 1 : 0);
        ++currentPaintDataIndex;
        };

    // 仅下限检查（负极标识检测，越大越好）
    auto checkLowerOnly = [&](const QString& name, float value, const QString& keyLower) {
        if (currentPaintDataIndex >= m_PaintData.size() || !m_PaintData[currentPaintDataIndex].check) {
            setPaintDataItemNoCheck();
            return;
        }

        float lower = getThresholdValue(keyLower);
        float comp = 0.0f;
        if (auto p = getProcessedParam(keyLower)) comp = p->compensationValue;

        float adjLower = lower - comp;  // 下限补偿放宽
        bool passed = std::isnan(lower) || value >= adjLower;

        printCheckInfo(name, value, adjLower, false, !passed);
        hasAnyFail |= !passed;
        updatePaintDataItem(value, passed ? 1 : 0);
        ++currentPaintDataIndex;
        };

    // ==== 按照 initStampPaintVector 顺序检测 ====

    checkRange("电容角度", ret.phi, "capacitor_angle_lower", "capacitor_angle_upper");
    checkRange("正极宽度", ret.posWidth, "positive_width_lower", "positive_width_upper");
    checkUpperOnly("正极瑕疵总面积", ret.posErr, "positive_defect_area");
    checkRange("负极宽度", ret.negWidth, "negative_width_lower", "negative_width_upper");
    checkUpperOnly("负极瑕疵总面积", ret.negErr, "negative_defect_area");
    checkLowerOnly("负极标识", ret.logoScores, "negative_mark");


    qDebug() << "m_PaintData[currentPaintDataIndex].value.toInt() " << m_PaintData[currentPaintDataIndex].value.toInt();
    // 字符个数（无判断，仅显示）
    if (LearnPara::inNum != ret.textNum)
    {

       
        hasAnyFail = 1;
        updatePaintDataItem(ret.textNum, 0);
        currentPaintDataIndex++;

 }
    else
    {
     
        updatePaintDataItem(ret.textNum, 1);
        currentPaintDataIndex++;
    }
    
    // 字符得分 character1 ~ character15（不使用补偿值）
    for (int i = 0; i < ret.textNum; ++i) {
        QString key = QString("character%1").arg(i + 1);
        QString name = QString("字符%1").arg(i + 1);

        if (currentPaintDataIndex >= m_PaintData.size()) {
            qWarning() << "judge_stamp - 字符得分索引超出范围，跳过。索引: " << currentPaintDataIndex;
            break;
        }

     //  if (m_PaintData[currentPaintDataIndex].check)
      // {
            if (i < ret.textScores.size())
            {
                float val = ret.textScores[i];
                float lower = getThresholdValue(key);
                bool passed = std::isnan(lower) || val >= lower;
                printCheckInfo(name, val, lower, false, !passed);
                hasAnyFail |= !passed;
                m_PaintData[currentPaintDataIndex].value = QString::number(val, 'f', 3);
                m_PaintData[currentPaintDataIndex].result = passed ? 1 : 0;
                ++currentPaintDataIndex;
            }
            else 
            {
                qWarning() << "judge_stamp - 字符得分数据不足，字符" << (i + 1) << "显示空值。";
                m_PaintData[currentPaintDataIndex].value = "";
                m_PaintData[currentPaintDataIndex].result = 1;
                ++currentPaintDataIndex;
            }
       // }
       // else 
      //  {
     //       setPaintDataItemNoCheck();
      //  }
    }



    if (currentPaintDataIndex != m_PaintData.size()) {
        qWarning() << "judge_stamp - PaintData 索引不匹配，已处理:" << currentPaintDataIndex << " 总大小:" << m_PaintData.size();
    }

    qDebug() << (hasAnyFail ? "--- 一个或多个检测项未通过 ---" : "--- 所有检测项均通过 ---");

    return hasAnyFail ? -1 : 0;
}

#endif
