#include "rezultinfo_xs.h"
#include <QDebug>

RezultInfo_Xs::RezultInfo_Xs(AllUnifyParams& unifyParams, QObject* parent)
    : RezultInfo() // 如果基类构造函数需要参数，请在此处传递
{
    this->unifyParams = unifyParams;
}

void RezultInfo_Xs::printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange)
{
    QString status = outOfRange ? "FAIL" : "PASS";
    QString limitType = isUpperLimit ? "Max" : "Min";

    qDebug() << status << ":" << paramName
        << "| Actual:" << actualValue
        << "|" << limitType << ":" << thresholdValue;
}

int RezultInfo_Xs::judge_xs(const XSResult& ret)
{
    // 注意：在调用 judge 之前，必须确保 config.value 已通过 updateActualValues 函数更新。
    // updateActualValues 负责将 XSResult 中的数据（包括枚举）填入 config.value。

    qDebug() << "--- START: 判定参数 (XS) ---";
    bool allPassed = true;

    // 1. 遍历成员变量 unifyParams 中的所有配置项
    for (auto it = unifyParams.begin(); it != unifyParams.end(); ++it)
    {
        const QString paramKey = it.key();
        UnifyParam& config = it.value(); // 必须使用非 const 引用来修改 config

        // 1.1 检查是否启用了检测
        if (!config.check) {
            config.result = 1; // 未启用检测，默认通过
            // qDebug() << "  -> Skipping disabled:" << config.label;
            continue;
        }

        // 1.2 统计更新：增加总检测次数
        config.count++;

        bool checkResult = false;

        qDebug() << "--- JUDGING (" << config.count << "):" << config.label << " (" << paramKey << ") ---";

        // 2. 匹配和判定 (根据 paramKey 严格匹配变量名)

        // -------------------------------------------------------------
        // 【通用数值检测】: 显式调用 config.checkRange()
        // -------------------------------------------------------------
        if (paramKey == "NGResult" ||        // 算法输出 (枚举转数值)
            paramKey == "JM_height" ||       // JM高度
            paramKey == "SHUJIAO_Width" ||   // 塑胶宽度
            paramKey == "ZW_ALLArea" ||      // 脏污总面积
            paramKey == "ZW_MaxArea" ||      // 脏污最大面积
            paramKey == "AX_MaxArea" ||      // 凹陷最大面积
            paramKey == "CMAX_MaxArea" ||   //凹陷面积
            paramKey == "HS_MaxArea")        // 划伤最大面积
        {
            qDebug() << "  -> Executing: checkRange()";
            // 调用 checkRange() 检测 config.value 是否在配置的上下限范围内
            checkResult = config.checkRange();
        }

        // --- 未匹配 ---
        else
        {
            qWarning() << "警告 (XS Judge): 配置项" << paramKey << "未在 XSResult 匹配列表中找到，跳过判定。";
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

    qDebug() << "--- END: 判定完成 (XS) --- Result:" << (allPassed ? 0 : 1);

    // 返回整数结果：0 表示全部通过 (OK)，1 表示存在 NG
    return allPassed ? 0 : 1;
}