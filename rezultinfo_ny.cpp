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

        // 2. 匹配和判定 (使用 config.checkRange() 进行范围判定)

        // 判断六个面积变量
        if (paramKey == "CQ_Area" ||
            paramKey == "GS_Area" ||
            paramKey == "HS_Area" ||
            paramKey == "QP_Area" ||
            paramKey == "YH_Area" ||
            paramKey == "ZW_Area")
        {
            // 判定 config.value (实际测量值) 是否在 JSON 配置的 [下限, 上限] 范围内
            checkResult = config.checkRange();
        }

        // --- 未匹配 (注意：原有的 "NGResult" 判断已被移除) ---
        else
        {
            qWarning() << "警告 (NY Judge): 配置项" << paramKey << "未在指定面积判定列表中找到，跳过判定。";
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
      //      qCritical().nospace() << "  -> !!! FAILED:" << config.label << " (" << paramKey << ") (实测值: " << config.value.toString()
         //       << ") 未通过检测 !!!";
            config.result = 0; // 不通过 (NG)
            config.ng_count++;
            allPassed = false;
        }
    }

    qDebug() << "--- END: 判定完成 (NY) --- Result:" << (allPassed ? 0 : 1);
    return allPassed ? 0 : 1;
}