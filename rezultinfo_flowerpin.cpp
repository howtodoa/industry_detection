#include "rezultinfo_flowerpin.h"

RezultInfo_FlowerPin::RezultInfo_FlowerPin(AllUnifyParams& unifyParams, QObject* parent)
    : RezultInfo()
{
    this->unifyParams = unifyParams;
}



void RezultInfo_FlowerPin::printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange)
{
    QString status = outOfRange ? "FAIL" : "PASS";
    QString limitType = isUpperLimit ? "Max" : "Min";

    qDebug() << status << ":" << paramName
        << "| Actual:" << actualValue
        << "|" << limitType << ":" << thresholdValue;
}

void RezultInfo_FlowerPin::applyScaleFactors(double scale) // 函数名保持不变
{
    qDebug() << "--- START: 应用缩放因子 (New Logic Debug) ---"; // 修改日志标识
    qDebug() << "  -> Global Scale Passed In:" << scale;

    // 遍历成员变量 unifyParams 中的所有配置项
    for (auto it = this->unifyParams.begin(); it != this->unifyParams.end(); ++it)
    {
        UnifyParam& config = it.value(); // 使用非 const 引用来修改 config
        const QString paramKey = it.key(); // 获取 key 用于日志

        // 记录原始值
        double originalValue = config.value;

        // --- 打印基础信息 ---
        qDebug().nospace() << "Checking Param: " << config.label << " (" << paramKey << ")";
        qDebug().nospace() << "  -> Before Scaling: config.value = " << originalValue;
        qDebug().nospace() << "  -> config.scaleFactor = " << config.scaleFactor;
        qDebug().nospace() << "  -> config.lowerLimit = " << config.lowerLimit;
        qDebug().nospace() << "  -> config.upperLimit = " << config.upperLimit;
        // ---

        // --- 【核心逻辑】判断是否应该跳过 ---
        // 使用 qFuzzyCompare 安全比较 double
        bool lowerIsUnset = qFuzzyCompare(config.lowerLimit, UNIFY_UNSET_VALUE);
        bool upperIsUnset = qFuzzyCompare(config.upperLimit, UNIFY_UNSET_VALUE);
        bool shouldSkip = lowerIsUnset && upperIsUnset; // 只有两者都 unset 才跳过
        qDebug() << "  -> Condition (lower == UNSET && upper == UNSET) Result:" << shouldSkip;
        // ---

        if (shouldSkip)
        {
            qDebug() << "    -> Condition BOTH UNSET met. Skipping scaling.";
            qDebug().nospace() << "    -> LOG: Skipped: " << config.label
                << " | Value remains: " << originalValue; // 保持原始值
        }
        else // 不跳过，需要应用缩放
        {
            qDebug() << "    -> Condition NOT both UNSET. Applying scaling...";

            // --- 【核心逻辑】确定因子 ---
            // 使用 qFuzzyCompare 安全比较 double
            bool useGlobalScale = qFuzzyCompare(config.scaleFactor, 1.0);
            double factor = useGlobalScale ? scale : config.scaleFactor;
            qDebug() << "    -> Use Global Scale (" << scale << ") ?" << useGlobalScale;
            qDebug().nospace() << "    -> Calculated Factor = " << factor;
            // ---

            // --- 1. 执行 config.value 的乘法操作 ---
            config.value *= factor;
            qDebug().nospace() << "      -> After config.value Scaling: config.value = " << config.value;
            // ---

            // --- 2. 【新增逻辑】处理 extraData 数组的缩放 ---
            if (config.extraData.isValid() && config.extraData.type() == QVariant::List)
            {
                QVariantList list = config.extraData.toList();
                QVariantList scaledList;
                scaledList.reserve(list.size());

                qDebug().nospace() << "      -> Scaling ExtraData Array (Size: " << list.size() << ") by Factor: " << factor;

                for (const QVariant& variant : list)
                {
                    // 假设 extraData 存储的是 float/double 类型，使用 toDouble() 确保精度
                    double elementValue = variant.toDouble();
                    double scaledElement = elementValue * factor;

                    // 将缩放后的 double 值重新添加到列表中
                    scaledList.append(scaledElement);
                }

                config.extraData = scaledList;
                qDebug() << "      -> ExtraData array successfully scaled.";
            }

            // 打印日志
            qDebug().nospace() << "      -> LOG: " << config.label
                << " | Original: " << originalValue
                << " * Factor: " << factor
                << " = Final Scaled: " << config.value;
        }
    }

    qDebug() << "--- END: 缩放因子应用完成 (New Logic Debug) ---"; // 修改日志标识
}

int RezultInfo_FlowerPin::judge_flower_pin(const OutFlowerPinResParam& ret)
{
    // 注意：在调用 judge 之前，必须确保 config.value 和 config.extraData 已经被 updateActualValues 更新。

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

        // 2. 匹配和判定 (根据 paramKey 显式调用独立的检测函数)

        // -------------------------------------------------------------
        // 【向量/数组检测项】: 显式调用 config.checkQListRange()
        // -------------------------------------------------------------
        if (paramKey == "flowerArea")
        {
            qDebug() << "  -> Executing: checkQListRange() for flowerArea";
            checkResult = config.checkQListRange();
        }
        else if (paramKey == "flowetLength")
        {
            qDebug() << "  -> Executing: checkQListRange() for flowetLength";
            checkResult = config.checkQListRange();
        }
        else if (paramKey == "allFlowerLength")
        {
            qDebug() << "  -> Executing: checkQListRange() for allFlowerLength";
            checkResult = config.checkQListRange();
        }

        // -------------------------------------------------------------
        // 【单值检测项】: 显式调用 config.checkRange()
        // -------------------------------------------------------------
        else if (
            // JSON中原本存在的单值项
            paramKey == "flowerNum" ||
            paramKey == "areaFoil" ||
            paramKey == "disFlw2L" ||
            paramKey == "disFlw2Pin" ||
            paramKey == "disPinAngle" ||
            paramKey == "disL2Heigh" ||
            // 【新增项】结构体中存在的单值项
            paramKey == "disFlw2Pin2" ||
            paramKey == "disFlowerAngle"
            // 注意：如果您最终决定保留 disL2Pin，请在这里添加 || paramKey == "disL2Pin"
            )
        {
            qDebug() << "  -> Executing: checkRange() (Single Value)";
            checkResult = config.checkRange();
        }

        // --- 未匹配 ---
        else
        {
            qWarning() << "警告: 配置项" << paramKey << "未在 FlowerPin 匹配列表中找到，跳过判定。";
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
        }
    }

    // 返回整数结果：0 为全部通过，1 为存在 NG
    return allPassed ? 0 : 1;
}