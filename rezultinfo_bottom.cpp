#include "rezultinfo_bottom.h"
#include <QDebug>

RezultInfo_Bottom::RezultInfo_Bottom(AllUnifyParams& unifyParams, QObject* parent)
    : RezultInfo() // 调用基类构造函数
{
    this->unifyParams = unifyParams;
}

void RezultInfo_Bottom::printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange)
{
    QString status = outOfRange ? "FAIL" : "PASS";
    QString limitType = isUpperLimit ? "Max" : "Min";

    qDebug() << status << ":" << paramName
        << "| Actual:" << actualValue
        << "|" << limitType << ":" << thresholdValue;
}

int RezultInfo_Bottom::judge_bottom(const Crop_BottomResult& ret)
{
    // 注意：在调用 judge 之前，必须确保 config.value 已通过 updateActualValues 函数更新。
    qDebug() << "--- START: 判定参数 (Bottom) ---";
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

        // 1.2 统计更新
        config.count++;
        bool checkResult = false;

        qDebug() << "--- JUDGING (" << config.count << "):" << config.label << " (" << paramKey << ") ---";

        // 2. 匹配和判定 (所有项均使用 checkRange 进行数值范围判定)
        if (paramKey == "NGResult" ||      // 算法结果(枚举转数值)
            paramKey == "JM_PSallArea" ||  // JM破损总面积
            paramKey == "JM_PSmaxArea" ||  // JM破损最大面积
            paramKey == "WB_MaxArea" ||    // 丸棒最大面积
            paramKey == "LK_PSallArea" ||  // 铝壳破损总面积
            paramKey == "LK_PSmaxArea")    // 铝壳破损最大面积
        {
            qDebug() << "  -> Executing: checkRange()";
            checkResult = config.checkRange();
        }

        // --- 未匹配 ---
        else
        {
            qWarning() << "警告 (Bottom Judge): 配置项" << paramKey << "未在 Crop_BottomResult 匹配列表中找到，跳过判定。";
            continue;
        }

        // 3. 汇总结果和统计更新
        if (checkResult)
        {
            config.result = 1; // 通过 (OK)
            qDebug() << "  -> PASSED.";
        }
        else
        {
            qCritical() << "  -> !!! FAILED:" << config.label << " (" << paramKey << ") 未通过检测 !!!";
            config.result = 0; // 不通过 (NG)
            config.ng_count++; // 增加 NG 次数
            allPassed = false;
        }
    }

    qDebug() << "--- END: 判定完成 (Bottom) --- Result:" << (allPassed ? 0 : 1);

    // 返回整数结果：0 表示全部通过 (OK)，1 表示存在 NG
    return allPassed ? 0 : 1;
}