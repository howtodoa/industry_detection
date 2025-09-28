#include "rezultinfo_nayin.h"
#include <QStringList>
#include <QRandomGenerator>
RezultInfo_NaYin::RezultInfo_NaYin(Parameters* params, QObject *parent)
    : RezultInfo(params, parent) // 将 params 传递给父类 RezultInfo 的构造函数
{
    initStampPaintVector(params);
}


void RezultInfo_NaYin::printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange)
{
    QString status = outOfRange ? "FAIL" : "PASS";
    QString limitType = isUpperLimit ? "Max" : "Min";

    qDebug() << status << ":" << paramName
             << "| Actual:" << actualValue
             << "|" << limitType << ":" << thresholdValue;
}

/*
int RezultInfo_NaYin::judge_stamp(const  OutStampResParam &ret)
{
    const QMap<QString, float>& thresholds = this->getProcessedData();

    // 辅助函数：直接从 thresholds 中获取值，不再需要 contains() 检查
    auto getParamFloatValue = [&](const QString& paramName) -> float {
        return thresholds.value(paramName);
    };

    // --- 统一声明和初始化所有阈值变量 ---
    // 宽度检测
    float posWidthLower = getParamFloatValue("正极宽度检测(负偏差)");
    float posWidthUpper = getParamFloatValue("正极宽度检测(正偏差)");
    float negWidthLower = getParamFloatValue("负极宽度检测(负偏差)");
    float negWidthUpper = getParamFloatValue("负极宽度检测(正偏差)");

    // 瑕疵检测 (总面积)
    float posErrThreshold = getParamFloatValue("正极瑕疵检测(总面积)");
    float negErrThreshold = getParamFloatValue("负极瑕疵检测(总面积)");

    // 负极标识检测 (分数)
    float logoScoresThreshold = getParamFloatValue("负极标识检测");

    // 电容角度检测
    float phiLower = getParamFloatValue("电容角度检测(下限)");
    float phiUpper = getParamFloatValue("电容角度检测(上限)");


    // --- 逐项判断逻辑 ---

    // 1. 正负极宽度检测
    // 正极宽度检测(正偏差) 1, 正极宽度检测(负偏差) 10, Actual: 5

    if (!std::isnan(posWidthLower) || !std::isnan(posWidthUpper)) {
        bool outOfRange = false;
        float failingThreshold = 0.0f; // 存储导致失败的阈值
        bool isUpper = false; // 存储是否是上限导致失败

        // 判断配置是否合法：下限是否小于等于上限
        bool isConfigValid = true;
        if (!std::isnan(posWidthLower) && !std::isnan(posWidthUpper) && posWidthLower > posWidthUpper) {
            isConfigValid = false;
            qWarning() << "WARNING: 正极宽度检测 config invalid! Lower limit (" << posWidthLower
                       << ") is greater than Upper limit (" << posWidthUpper << "). Marking as FAIL.";
            // 配置无效，直接判失败
            outOfRange = true;
            // 随便选择一个作为失败阈值，这里选上限
            failingThreshold = posWidthUpper;
            isUpper = true;
        }

        // 只有当配置合法且当前未被标记为失败时，才进行正常的范围判断
        if (isConfigValid && !outOfRange) {
            // 检查是否低于下限
            if (!std::isnan(posWidthLower) && ret.posWidth < posWidthLower) {
                outOfRange = true;
                failingThreshold = posWidthLower;
                isUpper = false; // 失败是由于低于下限
            }
            // 检查是否高于上限
            if (!std::isnan(posWidthUpper) && ret.posWidth > posWidthUpper) {
                // 注意：如果实际值既低于下限又高于上限（在无效范围内），这里会第二次触发 outOfRange = true，并更新 failingThreshold 为上限。
                // 这取决于你希望报告哪个失败。当前逻辑会优先报告上限失败如果两个都触发了。
                outOfRange = true;
                failingThreshold = posWidthUpper;
                isUpper = true; // 失败是由于高于上限
            }
        }

        printCheckInfo("正极宽度检测", ret.posWidth, failingThreshold, isUpper, outOfRange);
        if (outOfRange) return -1;
    } else {
        qDebug() << "INFO: 正极宽度检测 parameters not checked. Skipping.";
    }

    // 负极宽度检测 - 逻辑同正极宽度检测
    if (!std::isnan(negWidthLower) || !std::isnan(negWidthUpper)) {
        bool outOfRange = false;
        float failingThreshold = 0.0f;
        bool isUpper = false;

        bool isConfigValid = true;
        if (!std::isnan(negWidthLower) && !std::isnan(negWidthUpper) && negWidthLower > negWidthUpper) {
            isConfigValid = false;
            qWarning() << "WARNING: 负极宽度检测 config invalid! Lower limit (" << negWidthLower
                       << ") is greater than Upper limit (" << negWidthUpper << "). Marking as FAIL.";
            outOfRange = true;
            failingThreshold = negWidthUpper;
            isUpper = true;
        }

        if (isConfigValid && !outOfRange) {
            if (!std::isnan(negWidthLower) && ret.negWidth < negWidthLower) {
                outOfRange = true;
                failingThreshold = negWidthLower;
                isUpper = false;
            }
            if (!std::isnan(negWidthUpper) && ret.negWidth > negWidthUpper) {
                outOfRange = true;
                failingThreshold = negWidthUpper;
                isUpper = true;
            }
        }
        printCheckInfo("负极宽度检测", ret.negWidth, failingThreshold, isUpper, outOfRange);
        if (outOfRange) return -1;
    } else {
        qDebug() << "INFO: 负极宽度检测 parameters not checked. Skipping.";
    }


    // 2. 瑕疵检测 (总面积 - 越小越好，判断是否超过上限)
    if (!std::isnan(posErrThreshold)) {
        bool outOfRange = (ret.posErr > posErrThreshold);
        printCheckInfo("正极瑕疵检测(总面积)", ret.posErr, posErrThreshold, true, outOfRange); // true表示是上限
        if (outOfRange) return -1;
    } else {
        qDebug() << "INFO: 正极瑕疵检测(总面积) parameter not checked. Skipping.";
    }

    if (!std::isnan(negErrThreshold)) {
        bool outOfRange = (ret.negErr > negErrThreshold);
        printCheckInfo("负极瑕疵检测(总面积)", ret.negErr, negErrThreshold, true, outOfRange);
        if (outOfRange) return -1;
    } else {
        qDebug() << "INFO: 负极瑕疵检测(总面积) parameter not checked. Skipping.";
    }

    // 3. 负极标识检测 (分数 - 越大越好，判断是否低于下限)
    if (!std::isnan(logoScoresThreshold)) {
        bool outOfRange = (ret.logoScores < logoScoresThreshold);
        printCheckInfo("负极标识检测", ret.logoScores, logoScoresThreshold, false, outOfRange); // false表示是下限
        if (outOfRange) return -1;
    } else {
        qDebug() << "INFO: 负极标识检测 parameter not checked. Skipping.";
    }


    // 4. 电容角度检测 (有上下限)
    if (!std::isnan(phiLower) || !std::isnan(phiUpper)) {
        bool outOfRange = false;
        float failingThreshold = 0.0f;
        bool isUpper = false;

        bool isConfigValid = true;
        if (!std::isnan(phiLower) && !std::isnan(phiUpper) && phiLower > phiUpper) {
            isConfigValid = false;
            qWarning() << "WARNING: 电容角度检测 config invalid! Lower limit (" << phiLower
                       << ") is greater than Upper limit (" << phiUpper << "). Marking as FAIL.";
            outOfRange = true;
            failingThreshold = phiUpper;
            isUpper = true;
        }

        if (isConfigValid && !outOfRange) {
            if (!std::isnan(phiLower) && ret.phi < phiLower) {
                outOfRange = true;
                failingThreshold = phiLower;
                isUpper = false;
            }
            if (!std::isnan(phiUpper) && ret.phi > phiUpper) {
                outOfRange = true;
                failingThreshold = phiUpper;
                isUpper = true;
            }
        }
        printCheckInfo("电容角度检测", ret.phi, failingThreshold, isUpper, outOfRange);
        if (outOfRange) return -1;
    } else {
        qDebug() << "INFO: 电容角度检测 parameters not checked. Skipping.";
    }


    // 5. 字符分数 (越大越好，判断是否低于下限)
    for (int i = 0; i < ret.textScores.size(); ++i) {
        QString charName = QString("字符%1").arg(i + 1);
        float charScoreThreshold = getParamFloatValue(charName);
        if (!std::isnan(charScoreThreshold)) {
            bool outOfRange = (ret.textScores[i] < charScoreThreshold);
            printCheckInfo(charName, ret.textScores[i], charScoreThreshold, false, outOfRange); // false表示是下限
            if (outOfRange) return -1;
        } else {
            qDebug() << "INFO: " << charName << " parameter not checked. Skipping.";
        }
    }

    qDebug() << "--- All checks passed! ---";
    return 0; // 所有检测都通过
}
*/



void RezultInfo_NaYin::updatePaintData(Parameters *params)
{
    initStampPaintVector(params);
}

int RezultInfo_NaYin::judge_stamp_min(const OutStampResParam& ret)
{
    return 0;
}



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
