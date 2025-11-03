#include "rezultinfo_look.h"

RezultInfo_Look::RezultInfo_Look(AllUnifyParams& unifyParams, QObject* parent)
    : RezultInfo()
{
    this->unifyParams = unifyParams;
}



void RezultInfo_Look::printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange)
{
    QString status = outOfRange ? "FAIL" : "PASS";
    QString limitType = isUpperLimit ? "Max" : "Min";

    qDebug() << status << ":" << paramName
        << "| Actual:" << actualValue
        << "|" << limitType << ":" << thresholdValue;
}

int RezultInfo_Look::judge_look(const OutLookPinResParam& ret)
{
    // 注意：在调用 judge 之前，必须确保 config.value 已通过 updateActualValues 函数更新。

    bool allPassed = true;

    // 1. 遍历成员变量 unifyParams 中的所有配置项
    for (auto it = unifyParams.begin(); it != unifyParams.end(); ++it)
    {
        const QString paramKey = it.key();
        UnifyParam& config = it.value(); // 必须使用非 const 引用来修改 config

        // 1.1 检查是否启用了检测
        if (!config.check) {
            config.result = 1; // 未启用检测，默认通过
            continue;
        }

        // 1.2 统计更新：增加总检测次数
        config.count++;

        bool checkResult = false;

        qDebug() << "--- JUDGING (" << config.count << "):" << config.label << "---";

        // 2. 匹配和判定 (根据 paramKey 严格匹配变量名)

        // -------------------------------------------------------------
        // 【单值检测项】: 显式调用 config.checkRange()
        // -------------------------------------------------------------
        if (paramKey == "packageHeight" ||
            paramKey == "packageWidth" ||
            paramKey == "pinDistance" ||
            paramKey == "pindiff")
        {
            qDebug() << "  -> Executing: checkRange() (Single Value)";
            // 调用 checkRange() 检测 config.value 是否在配置的上下限范围内
            checkResult = config.checkRange();
        }

        // --- 未匹配 ---
        else
        {
            qWarning() << "警告: 配置项" << paramKey << "未在 LookPin 匹配列表中找到，跳过判定。";
            continue;
        }

        // 3. 汇总结果和统计更新
        if (checkResult)
        {
            config.result = 1; // 通过 (OK)
        }
        else
        {
            qCritical() << "!!! FAILED:" << config.label << "未通过检测 !!!";
            config.result = 0; // 不通过 (NG)
            config.ng_count++; // 增加 NG 次数
            allPassed = false;
        }
    }

    // 返回整数结果：0 表示全部通过 (OK)，1 表示存在 NG
    return allPassed ? 0 : 1;
}