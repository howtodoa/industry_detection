#include "rezultinfo_flowerpin.h"

RezultInfo_Abut::RezultInfo_Abut(AllUnifyParams& unifyParams, QObject* parent)
    : RezultInfo()
{
    this->unifyParams = unifyParams;
}



void RezultInfo_Abut::printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange)
{
    QString status = outOfRange ? "FAIL" : "PASS";
    QString limitType = isUpperLimit ? "Max" : "Min";

    qDebug() << status << ":" << paramName
        << "| Actual:" << actualValue
        << "|" << limitType << ":" << thresholdValue;
}

int RezultInfo_Abut::judge_abut(const OutAbutResParam& ret)
{


    bool allPassed = true;

    // 1. 遍历成员变量 unifyParams 中的所有配置项
    for (auto it = unifyParams.begin(); it != unifyParams.end(); ++it)
    {
        const QString paramKey = it.key();
        UnifyParam& config = it.value(); // !!! 必须使用非 const 引用来修改 config !!!

        // 1.1 检查是否启用了检测
        if (!config.check) {
            config.result = 1; // 未启用检测，默认通过
            continue;
        }

        // 1.2 统计更新：增加总检测次数
        config.count++;

        bool checkResult = false;

        qDebug() << "--- JUDGING (" << config.count << "):" << config.label << "---";

        // 2. 执行您指定的 If-Else If 匹配和判定 (调用结构体内部函数)

        // --- 布尔检测项：调用 config.checkBool() ---
        if (paramKey == "isHaveProd") {
            // 注意：config.value 已经被 updateActualValues 赋值为 ret.isHaveProd
            checkResult = config.checkBool();
        }
        else if (paramKey == "isHavePpin") {
            checkResult = config.checkBool();
        }
        else if (paramKey == "isHaveNpin") {
            checkResult = config.checkBool();
        }
        else if (paramKey == "isHaveBpln") {
            checkResult = config.checkBool();
        }

        // --- 数值检测项：调用 config.checkRange() ---
        else if (paramKey == "Pin_C") {
            // 注意：config.value 已经被 applyScaleFactors 缩放
            checkResult = config.checkRange();
        }
        else if (paramKey == "shuyao_width") {
            checkResult = config.checkRange();
        }
        else if (paramKey == "plate_width") {
            checkResult = config.checkRange();
        }
        else if (paramKey == "p_pin_over_pln") {
            checkResult = config.checkRange();
        }
        else if (paramKey == "n_pin_over_pln") {
            checkResult = config.checkRange();
        }
        else if (paramKey == "p_pin_H") {
            checkResult = config.checkRange();
        }
        else if (paramKey == "n_pin_H") {
            checkResult = config.checkRange();
        }
        else if (paramKey == "p_n_height_diff") {
            checkResult = config.checkRange();
        }
        else if (paramKey == "p_pin_Angle") {
            checkResult = config.checkRange();
        }
        else if (paramKey == "n_pin_Angle") {
            checkResult = config.checkRange();
        }
        else if (paramKey == "p_pin_Mc") {
            checkResult = config.checkRange();
        }
        else if (paramKey == "n_pin_Mc") {
            checkResult = config.checkRange();
        }
        else if (paramKey == "b_pln_Mc") {
            checkResult = config.checkRange();
        }
        else
        {
            qWarning() << "警告: 配置项" << paramKey << "未在匹配列表中找到，跳过判定。";
            // 匹配失败的项，不进行后续结果处理，但需要恢复 allPassed 状态
            continue;
        }

        // 3. 汇总结果和统计更新
        if (checkResult)
        {
            config.result = 1; // 通过
        }
        else
        {
            qCritical() << "!!! FAILED:" << config.label << "未通过检测 !!!";
            config.result = 0; // 不通过
            config.ng_count++; // 增加 NG 次数
            allPassed = false;
            // 注意：如果您希望发现 NG 后立即返回，可以在这里添加 return 1;
        }
    }

    // 返回整数结果：0 为全部通过，1 为存在 NG
    return allPassed ? 0 : 1;
}