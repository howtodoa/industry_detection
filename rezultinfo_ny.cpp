#include "rezultinfo_ny.h"
#include <QDebug>

RezultInfo_Ny::RezultInfo_Ny(AllUnifyParams& unifyParams, QObject* parent)
    : RezultInfo() // 调用基类构造函数
{
    this->unifyParams = unifyParams;
}

void RezultInfo_Ny::printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange)
{
    QString status = outOfRange ? "FAIL" : "PASS";
    QString limitType = isUpperLimit ? "Max" : "Min";

    qDebug() << status << ":" << paramName
        << "| Actual:" << actualValue
        << "|" << limitType << ":" << thresholdValue;
}

int RezultInfo_Ny::judge_ny(const NYResult& ret)
{
    // 注意：调用此函数前，必须确保 updateActualValues 已经执行，config.value 已被赋值
    qDebug() << "--- START: 判定参数 (NY) ---";
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

        // 2. 匹配和判定
        if (paramKey == "NGResult")
        {
            // 判定 config.value (枚举转成的数值) 是否在范围 "0-0" (即 OK) 内
            checkResult = config.checkRange();
        }
        // --- 未匹配 ---
        else
        {
            qWarning() << "警告 (NY Judge): 配置项" << paramKey << "未在 NYResult 匹配列表中找到，跳过判定。";
            continue;
        }

        // 3. 汇总结果
        if (checkResult)
        {
            config.result = 1; // 通过 (OK)
            qDebug() << "  -> PASSED.";
        }
        else
        {
            qCritical() << "  -> !!! FAILED:" << config.label << " (" << paramKey << ") 未通过检测 !!!";
            config.result = 0; // 不通过 (NG)
            config.ng_count++;
            allPassed = false;
        }
    }

    qDebug() << "--- END: 判定完成 (NY) --- Result:" << (allPassed ? 0 : 1);
    return allPassed ? 0 : 1;
}