#include "rezultinfo.h"
#include <QMap>
#include <QVariant>



RezultInfo::RezultInfo(Parameters *rangepara, QObject *parent)
    : QObject(parent),score_pos(15,0.65f),score_neg(1,0.65f)
{
    processRangeParameters(rangepara);
    m_PaintData.reserve(30);
}


RezultInfo::~RezultInfo()
{

}

const QList<ProcessedParam>& RezultInfo::getProcessedData() const
{
    return m_processedData;
}


void RezultInfo::printPaintDataItems()
{
    qDebug() << "--- 打印 RezultInfo::m_PaintData 成员变量 ---";

    if (m_PaintData.isEmpty()) {
        qDebug() << "m_PaintData 列表为空，没有可打印的项。";
        return;
    }

    for (int i = 0; i < m_PaintData.size(); ++i) {
        const PaintDataItem& item = m_PaintData.at(i);

        qDebug() << "项目 " << (i + 1) << ":";
        qDebug() << "  label:  " << item.label;
        qDebug() << "  check:  " << (item.check ? "true" : "false");
        qDebug() << "  value:  " << item.value;
        qDebug() << "  result: " << item.result;
    }
    qDebug() << "--- 打印结束 ---";
}

void RezultInfo::initLiftPaintVector(Parameters* rangePara)
{
    m_PaintData.clear();

    if (!rangePara) {
        qWarning() << "initLiftPaintVector: 参数为空";
        return;
    }

    const QMap<QString, DetectionProject>& projects = rangePara->detectionProjects;
    QMap<QString, ParamDetail> paramMap;

    for (const auto& project : projects) {
        for (auto it = project.params.begin(); it != project.params.end(); ++it) {
            const ParamDetail& detail = it.value();
            if (!detail.name.isEmpty()) {
                paramMap.insert(detail.name, detail);
            }
        }
    }

    auto getOffset = [&](const QString& keyLower, const QString& keyUpper) -> double {
        double offset = 0.0;

        if (paramMap.contains(keyLower)) {
            offset = paramMap[keyLower].compensation;
        }
        else if (paramMap.contains(keyUpper)) {
            offset = paramMap[keyUpper].compensation;
        }

        return offset;
        };

    auto addPaintItem = [&](const QString& keyLower, const QString& keyUpper, const QString& displayName) {
        bool hasLower = paramMap.contains(keyLower);
        bool hasUpper = paramMap.contains(keyUpper);

        bool check = false;
        if (hasLower) check |= paramMap[keyLower].check;
        if (hasUpper) check |= paramMap[keyUpper].check;

        double offset = getOffset(keyLower, keyUpper);

        if (hasLower || hasUpper) {
            m_PaintData.append({ displayName, check, "", -1, offset });
        }
        else {
            qWarning() << "缺少配置项:" << keyLower << "或" << keyUpper << "，加入占位项";
            m_PaintData.append({ displayName, false, "", -1, 0.0 });
        }
        };

    // ==== 顺序需严格与 OutLiftResParam 一致 ====
    addPaintItem("m_PinWidthLiftLower", "m_PinWidthLiftUpper", "引脚宽度");
    addPaintItem("m_PinHeightLiftLower", "m_PinHeightLiftUpper", "引脚高度");
    addPaintItem("m_disLiftLower", "m_disLiftUpper", "引脚到底座距离");
    addPaintItem("m_PinAngleLower", "m_PinAngleUpper", "引脚角度");

    qDebug() << "m_PaintData(Lift) 初始化完成，大小:" << m_PaintData.size();
}


void RezultInfo::initStampPaintVector(Parameters* rangePara)
{
    m_PaintData.clear();
    // 清空分数数组，确保每次调用时都是最新的
    score_pos.clear();
    score_neg.clear();

    if (!rangePara) {
        qWarning() << "initStampPaintVector: 参数为空";
        return;
    }

    // 获取所有检测项目
    const QMap<QString, DetectionProject>& projects = rangePara->detectionProjects;

    // 映射：映射变量名（name） -> ParamDetail
    QMap<QString, ParamDetail> paramMap;

    for (const auto& project : projects) {
        for (auto it = project.params.begin(); it != project.params.end(); ++it) {
            const ParamDetail& detail = it.value();
            if (!detail.name.isEmpty()) {
                paramMap.insert(detail.name, detail);
            }
        }
    }

    // ====== 初始化 PaintData 按 OutStampResParam 顺序 ======

    auto addPaintItem = [&](const QString& mapName, const QString& displayName) {
        if (paramMap.contains(mapName)) {
            const ParamDetail& detail = paramMap[mapName];
            m_PaintData.append({ displayName, detail.check });
         
        }
        else {
            qWarning() << "缺少配置项:" << mapName;
            m_PaintData.append({ displayName, false }); // 缺失也占位
        }
        };

    // 1. phi
    addPaintItem("capacitor_angle_lower", "电容角度");

    // 2. posWidth
    addPaintItem("positive_width_upper", "正极宽度");

    // 3. posErr
    addPaintItem("positive_defect_area", "正极瑕疵总面积");

    // 4. negWidth
    addPaintItem("negative_width_upper", "负极宽度");

    // 5. negErr
    addPaintItem("negative_defect_area", "负极瑕疵总面积");

    // 6. logoScores（注意顺序调整到 textScores 前）
    // 将 negative_mark 的值放入 score_neg
    if (paramMap.contains("negative_mark")) {
        const ParamDetail& detail = paramMap["negative_mark"];
        m_PaintData.append({ "负极标识检测", detail.check });
        score_neg.push_back(detail.value.toDouble()); 
    }
    else {
        qWarning() << "缺少配置项: negative_mark";
        m_PaintData.append({ "负极标识检测", false }); // 无配置也占位
        score_neg.push_back(0.656f); // 如果缺失，可以添加默认值
    }

    // 7. textNum(配置文件没添加，这个是按照学习结果来更新)
    m_PaintData.append({ "字符个数", true , QString::number(15)});
    
    // 8. textScores: character1 ~ character15
    for (int i = 1; i <= 15; ++i) {
        QString var = QString("character%1").arg(i);
        QString label = QString("字符%1").arg(i);
        if (paramMap.contains(var)) {
            const ParamDetail& detail = paramMap[var];
            m_PaintData.append({ label, true});//全部默认检测
            // 假设 ParamDetail 有一个 float 类型的 value 字段
            score_pos.push_back(detail.value.toDouble()); // 将字符1-15的值添加到 score_pos
        }
        else {
            m_PaintData.append({ label, false }); // 无配置也占位
            score_pos.push_back(0.656f); // 如果缺失，可以添加默认值
        }
    }
    for (int i = 0; i < LearnPara::inNum; i++)
    {
        m_PaintData[7 + i].check = true;
    }
    for (int i = LearnPara::inNum;i<15;i++)
    {
        m_PaintData[7 + i].check = false;
    }
    qDebug() << "m_PaintData 初始化完成，大小:" << m_PaintData.size();
    qDebug() << "score_pos 大小:" << score_pos.size();
    qDebug() << "score_neg 大小:" << score_neg.size();
}


void RezultInfo::initSidePaintVector(Parameters* rangePara)
{
    m_PaintData.clear();

    if (!rangePara) {
        qWarning() << "initSidePaintVector: 参数为空";
        return;
    }

    const QMap<QString, DetectionProject>& projects = rangePara->detectionProjects;
    QMap<QString, ParamDetail> paramMap;

    for (const auto& project : projects) {
        for (auto it = project.params.begin(); it != project.params.end(); ++it) {
            const ParamDetail& detail = it.value();
            if (!detail.name.isEmpty()) {
                paramMap.insert(detail.name, detail);
            }
        }
    }

    auto getOffset = [&](const QString& keyLower, const QString& keyUpper) -> double {
        double offset = 0.0;
        if (paramMap.contains(keyLower)) {
            offset = paramMap[keyLower].compensation;
        }
        else if (paramMap.contains(keyUpper)) {
            offset = paramMap[keyUpper].compensation;
        }
        return offset;
        };

    auto addPaintItem = [&](const QString& keyLower, const QString& keyUpper, const QString& displayName) {
        bool hasLower = paramMap.contains(keyLower);
        bool hasUpper = paramMap.contains(keyUpper);

        bool check = false;
        if (hasLower) check |= paramMap[keyLower].check;
        if (hasUpper) check |= paramMap[keyUpper].check;

        double offset = getOffset(keyLower, keyUpper);

        if (hasLower || hasUpper) {
            m_PaintData.append({ displayName, check, "", -1, offset });
        }
        else {
            qWarning() << "缺少配置项:" << keyLower << "或" << keyUpper << "，加入占位项";
            m_PaintData.append({ displayName, false, "", -1, 0.0 });
        }
        };

    auto addBoolItem = [&](const QString& key, const QString& displayName) {
        if (paramMap.contains(key)) {
            const ParamDetail& detail = paramMap[key];
            m_PaintData.append({ displayName, detail.check, "", -1, 0.0 });
        }
        else {
            qWarning() << "缺少布尔类型配置项:" << key << "，加入占位项";
            m_PaintData.append({ displayName, false, "", -1, 0.0 });
        }
        };
    auto addQStringItem = [&](const QString& key, const QString& displayName) {
        if (paramMap.contains(key)) {
            const ParamDetail& detail = paramMap[key];
            m_PaintData.append({ displayName, detail.check, "", -1, 0.0 });
        }
        else {
            qWarning() << "缺少字符串类型配置项:" << key << "，加入占位项";
            m_PaintData.append({ displayName, false, "", -1, 0.0 });
        }
        };
    // ==== 顺序需与 OutSideParam 严格一致 ====
    addQStringItem("m_VolumeStr","容积字符信息");
    addQStringItem("m_VoltageStr", "电压字符信息");
    addBoolItem("m_VolumeOK", "容积匹配");
    addBoolItem("m_VoltageOK", "电压匹配");
    addBoolItem("m_PinOK", "针脚状态");
    addBoolItem("m_BreakOK", "破损检测");
    addBoolItem("m_CaseColor", "套管颜色");
    addBoolItem("m_CaseOK", "套管状态");
    addPaintItem("m_PinDisLower", "m_PinDisUpper", "针脚间距");
    addPaintItem("m_PinWidthLower", "m_PinWidthUpper", "针脚宽度");

    addPaintItem("m_CaseHeighLower", "m_CaseHeighUpper", "套管高度");
    addPaintItem("m_Case2PinDisLower", "m_Case2PinDisUpper", "套管与针间距");
    addPaintItem("m_CaseYSiteLower", "m_CaseYSiteUpper", "套管Y差值");

    addPaintItem("m_TapeDisLower", "m_TapeDisUpper", "胶带间隔");
    addPaintItem("m_Case2TapeDisUpper", "m_Case2TapeDisLower", "套管与胶带间距");


    qDebug() << "m_PaintData(Side) 初始化完成，大小:" << m_PaintData.size();
}

QVector<SimpleParam> RezultInfo::initScale(QString ScalePath)
{
    QVector<SimpleParam> scaleArray;
    QVariantMap scaleDataMap = FileOperator::readJsonMap(ScalePath);

    if (scaleDataMap.isEmpty()) {
        qWarning() << "Failed to read JSON scale file or file is empty.";
        return scaleArray;
    }

    for (auto it = scaleDataMap.begin(); it != scaleDataMap.end(); ++it) {
        QVariant paramVariant = it.value();

        // 确保值是一个 QVariantMap，然后从中提取 "值"
        if (paramVariant.type() == QVariant::Map) {
            QVariantMap paramMap = paramVariant.toMap();
            SimpleParam param;
            param.name = it.key();

            // 正确访问嵌套的 "值" 键
            param.value = paramMap.value("值", 1.0).toDouble(); // 加上默认值，避免没有"值"的情况
            scaleArray.push_back(param);
        }
        else {
            qWarning() << "JSON value for" << it.key() << "is not a map. Skipping.";
        }
    }

    qDebug() << "Debug: Initialized ScaleArray content:";
    for (const auto& p : scaleArray) {
        qDebug() << "  Name:" << p.name << ", Value:" << p.value;
    }

    return scaleArray;
}

void RezultInfo::updatePaintDataFromScaleArray(const QVector<SimpleParam>& scaleArray)
{
    for (int i = 0; i < m_PaintData.size(); ++i) {
        const QString& label = m_PaintData[i].label;
        for (const SimpleParam& scaleParam : scaleArray) {
            if (label == scaleParam.name) {
                m_PaintData[i].scale = scaleParam.value;
                break;
            }
        }
    }
}

void RezultInfo::initTopPaintVector(Parameters* rangePara)
{
    m_PaintData.clear();

    if (!rangePara) {
        qWarning() << "initTopPaintVector: 参数为空";
        return;
    }

    const QMap<QString, DetectionProject>& projects = rangePara->detectionProjects;
    QMap<QString, ParamDetail> paramMap;

    // 收集所有项目的参数到 paramMap，以 name 为 key
    for (const auto& project : projects) {
        for (auto it = project.params.begin(); it != project.params.end(); ++it) {
            const ParamDetail& detail = it.value();
            if (!detail.name.isEmpty()) {
                paramMap.insert(detail.name, detail);
            }
        }
    }

    // 获取补偿值（优先使用下限项）
    auto getOffset = [&](const QString& keyLower, const QString& keyUpper) -> double {
        double offset = 0.0;
        if (paramMap.contains(keyLower)) {
            offset = paramMap[keyLower].compensation;
        }
        else if (paramMap.contains(keyUpper)) {
            offset = paramMap[keyUpper].compensation;
        }
        return offset;
        };

    // 添加普通上下限配对的绘图项
    auto addPaintItem = [&](const QString& keyLower, const QString& keyUpper, const QString& displayName) {
        bool hasLower = paramMap.contains(keyLower);
        bool hasUpper = paramMap.contains(keyUpper);

        bool check = false;
        if (hasLower) check |= paramMap[keyLower].check;
        if (hasUpper) check |= paramMap[keyUpper].check;

        double offset = getOffset(keyLower, keyUpper);

        if (hasLower || hasUpper) {
            m_PaintData.append({ displayName, check, "", -1, offset });
        }
        else {
            qWarning() << "缺少配置项:" << keyLower << "或" << keyUpper << "，加入占位项";
            m_PaintData.append({ displayName, false, "", -1, 0.0 });
        }
        };

    // 添加布尔类型检测项（不涉及上下限）
    auto addBoolItem = [&](const QString& key, const QString& displayName) {
        if (paramMap.contains(key)) {
            const ParamDetail& detail = paramMap[key];
            m_PaintData.append({ displayName, detail.check, "", -1, 0.0 });
        }
        else {
            qWarning() << "缺少布尔类型配置项:" << key << "，加入占位项";
            m_PaintData.append({ displayName, false, "", -1, 0.0 });
        }
        };

    // ==== 顺序需严格与 OutTopParam 一致 ====
    addBoolItem("m_FBFClassOKTop", "防爆阀类型");
    addPaintItem("m_NegAngleTopLower", "m_NegAngleTopUpper", "负极角度");
    addPaintItem("m_CirNumTopLower", "m_CirNumTopUpper", "防爆阀圆度");
    addPaintItem("m_RadiusTopLower", "m_RadiusTopUpper", "防爆阀半径");

    qDebug() << "m_PaintData(Top) 初始化完成，大小:" << m_PaintData.size();
}

void RezultInfo::initPlatePaintVector(Parameters* rangePara)
{
    m_PaintData.clear();

    if (!rangePara) {
        qWarning() << "initPlatePaintVector: 参数为空";
        return;
    }

    const QMap<QString, DetectionProject>& projects = rangePara->detectionProjects;
    QMap<QString, ParamDetail> paramMap;

    for (const auto& project : projects) {
        for (auto it = project.params.begin(); it != project.params.end(); ++it) {
            const ParamDetail& detail = it.value();
            if (!detail.name.isEmpty()) {
                paramMap.insert(detail.name, detail);
            }
        }
    }

    auto addPaintItem = [&](const QString& keyLower, const QString& keyUpper, const QString& displayName) {
        bool hasLower = paramMap.contains(keyLower);
        bool hasUpper = paramMap.contains(keyUpper);

        bool check = false;
        double offset = 0.0;

        QString valLower, valUpper;
        double compLower = 0.0, compUpper = 0.0;

        if (hasLower) {
            const auto& d = paramMap[keyLower];
            check |= d.check;
            compLower = d.compensation;
            offset = compLower;
            valLower = d.value.toString();
        }

        if (hasUpper) {
            const auto& d = paramMap[keyUpper];
            check |= d.check;
            compUpper = d.compensation;
            if (!hasLower) offset = compUpper;
            valUpper = d.value.toString();
        }

        m_PaintData.append({ displayName, check, "", -1, offset });

        qDebug() << "=== " << displayName << " ===";
        if (hasLower)
            qDebug() << "  下限键:" << keyLower << "值:" << valLower << "检测:" << (paramMap[keyLower].check ? "true" : "false") << "补偿:" << compLower;
        else
            qDebug() << "  下限键:" << keyLower << "（未找到）";

        if (hasUpper)
            qDebug() << "  上限键:" << keyUpper << "值:" << valUpper << "检测:" << (paramMap[keyUpper].check ? "true" : "false") << "补偿:" << compUpper;
        else
            qDebug() << "  上限键:" << keyUpper << "（未找到）";

        qDebug() << "  最终检测状态:" << (check ? "true" : "false") << "使用补偿:" << offset;
        };

    auto addExistencePaintItem = [&](const QString& key, const QString& displayName) {
        bool check = false;
        if (paramMap.contains(key)) {
            check = paramMap[key].check;
        }
        else {
            qWarning() << "缺少存在性检测项:" << key;
        }
        m_PaintData.append({ displayName, check, "", -1, 0.0 });

        qDebug() << "=== " << displayName << "(存在性检测) ===";
        qDebug() << "  键:" << key << " 检测:" << (check ? "true" : "false");
        };

    // 以下顺序严格对应 OutPlateResParam
    addExistencePaintItem("m_PlatExist", "底座存在");
    addPaintItem("m_PlateAreaLower", "", "底座面积");
    addPaintItem("m_PlateHeightLower", "m_PlateHeightUpper", "底座高度");
    addPaintItem("m_PlateWidLower", "m_PlateWidUpper", "底座宽度");
    addPaintItem("m_PlateHypUpLenLower", "m_PlateHypUpLenUpper", "座板上斜边长度");
    addPaintItem("m_PlateHypDownLenLower", "m_PlateHypDownLenUpper", "座板下斜边长度");

    addPaintItem("", "m_PlateErrNumUpper", "底座瑕疵区域");
    addPaintItem("", "m_PlateCrushNumUpper", "底座压伤区域");
 
    addExistencePaintItem("m_PlatePinLeftExist", "左引脚存在");
    addExistencePaintItem("m_PlatePinRightExist", "右引脚存在");

    addPaintItem("m_PlatePinLeftHeightLower", "m_PlatePinLeftHeightUpper", "左引脚高度");
    addPaintItem("m_PlatePinRightHeightLower", "m_PlatePinRightHeightUpper", "右引脚高度");

    addPaintItem("m_PlatePinLeftWidLower", "m_PlatePinLeftWidUpper", "左引脚宽度");
    addPaintItem("m_PlatePinRightWidLower", "m_PlatePinRightWidUpper", "右引脚宽度");

    addPaintItem("m_PlatePinLeftAngleLower", "m_PlatePinLeftAngleUpper", "左引脚角度");
    addPaintItem("m_PlatePinRightAngleLower", "m_PlatePinRightAngleUpper", "右引脚角度");

    addPaintItem("m_PlatePinLeftBendAngleLower", "m_PlatePinLeftBendAngleUpper", "左引脚弯脚");
    addPaintItem("m_PlatePinRightBendAngleLower", "m_PlatePinRightBendAngleUpper", "右引脚弯脚");

    addPaintItem("m_PlatePinLeftParalAngleLower", "m_PlatePinLeftParalAngleUpper", "左引脚平行度");
    addPaintItem("m_PlatePinRightParalAngleLower", "m_PlatePinRightParalAngleUpper", "右引脚平行度");

    addPaintItem("m_PlatePinLeftExceLenLower", "m_PlatePinLeftExceLenUpper", "左引脚超底座长度");
    addPaintItem("m_PlatePinRightExceLenLower", "m_PlatePinRightExceLenUpper", "右引脚超底座长度");

    addPaintItem("m_PlatePinTotalLenLower", "m_PlatePinTotalLenUpper", "引线总长度");

    qDebug() << "=== m_PaintData(Plate) 初始化完成 === 大小:" << m_PaintData.size();
}

void RezultInfo::initPinPaintVector(Parameters* rangePara)
{
    m_PaintData.clear();

    if (!rangePara) {
        qWarning() << "initPinPaintVector: 参数为空";
        return;
    }

    const QMap<QString, DetectionProject>& projects = rangePara->detectionProjects;
    QMap<QString, ParamDetail> paramMap;

    for (const auto& project : projects) {
        for (auto it = project.params.begin(); it != project.params.end(); ++it) {
            const ParamDetail& detail = it.value();
            if (!detail.name.isEmpty()) {
                paramMap.insert(detail.name, detail);
            }
        }
    }

    auto getOffset = [&](const QString& keyLower, const QString& keyUpper) -> double {
        if (paramMap.contains(keyLower))
            return paramMap[keyLower].compensation;
        if (paramMap.contains(keyUpper))
            return paramMap[keyUpper].compensation;
        return 0.0;
        };

    auto addRangeFloatItem = [&](const QString& keyLower, const QString& keyUpper, const QString& displayName) {
        bool hasLower = paramMap.contains(keyLower);
        bool hasUpper = paramMap.contains(keyUpper);

        bool check = false;
        if (hasLower) check |= paramMap[keyLower].check;
        if (hasUpper) check |= paramMap[keyUpper].check;

        double offset = getOffset(keyLower, keyUpper);

        if (hasLower || hasUpper) {
            m_PaintData.append({ displayName, check, "", -1, offset });
        }
        else {
            qWarning() << "缺少上下限配置项:" << keyLower << "/" << keyUpper << "，加入占位项";
            m_PaintData.append({ displayName, false, "", -1, 0.0 });
        }
        };

    auto addBoolItem = [&](const QString& key, const QString& displayName) {
        if (paramMap.contains(key)) {
            const ParamDetail& detail = paramMap[key];
            m_PaintData.append({ displayName, detail.check, "", -1, 0.0 });
        }
        else {
            qWarning() << "缺少布尔类型配置项:" << key << "，加入占位项";
            m_PaintData.append({ displayName, false, "", -1, 0.0 });
        }
        };

    // === 顺序必须与 OutPinParam 一致 ===
    addRangeFloatItem("m_MaoCiAreaLower", "m_MaoCiAreaUpper","毛刺面积");
    addRangeFloatItem("m_DisCircleLower", "m_DisCircleUpper", "定位圆间距");
    addRangeFloatItem("m_DisTopPinLower", "m_DisTopPinUpper", "上针宽度间距");
    addRangeFloatItem("m_DisButtomPinLower", "m_DisButtomPinUpper", "下针宽度间距");
    addRangeFloatItem("m_DisPinWidUpper", "m_DisPinWidUpper", "针最大宽度");
    addRangeFloatItem("m_totalAreaLower", "m_totalAreaUpper", "缺陷总面积");
    addRangeFloatItem("m_DisTapeWidUpper", "m_DisTapeWidUpper", "胶带宽度");

    qDebug() << "m_PaintData(Pin) 初始化完成，大小:" << m_PaintData.size();
}


float RezultInfo::getCompensationValue(const QString& mappedKey) const
{
    const ProcessedParam* param = getProcessedParam(mappedKey);
    if (param) {
        return static_cast<float>(param->compensationValue);
    }
    return 0.0f;
}


float RezultInfo::getThresholdValue(const QString& key) const
{
    for (const auto& param : m_processedData) {
        if (param.mappedVariable == key) {
            if (param.value.canConvert<float>()) {
                return param.value.toFloat() + static_cast<float>(param.compensationValue);
            }
            break;
        }
    }
    return std::numeric_limits<float>::quiet_NaN();
}
// 记录绘图数据
void RezultInfo::appendPaintData(const QString& name, bool check, float value, int result)
{
    PaintDataItem item;
    item.label = name;
    item.check = check;
    item.value = QString::number(value, 'f', 3); // float 转 QString，保留3位小数
    item.result = result;
    m_PaintData.append(item);
}

void RezultInfo::processRangeParameters(Parameters* rangePara)
{
    m_processedData.clear();

    if (!rangePara) {
        qWarning() << "参数为空，无法初始化 m_processedData";
        return;
    }

    const QMap<QString, DetectionProject>& projects = rangePara->detectionProjects;

    for (const auto& project : projects) {
        for (auto it = project.params.constBegin(); it != project.params.constEnd(); ++it) {
            const ParamDetail& detail = it.value();

            if (detail.name.isEmpty()) {
                qWarning() << "映射变量名为空，跳过参数：" << it.key();
                continue;
            }

            ProcessedParam p;
            p.originalName = it.key();
            p.mappedVariable = detail.name;
            p.value = detail.value;
            p.isChecked = detail.check;
            p.compensationValue = detail.compensation;

            m_processedData.append(p);
        }
    }

    qDebug() << "m_processedData 初始化完成，个数：" << m_processedData.size();
}


//void RezultInfo::printProcessedData() const
//{
//    qDebug() << "--- RezultInfo Processed Flat Key-Value Data (float/NaN) ---";
//    if (m_processedData.isEmpty()) {
//        qDebug() << "No processed data available.";
//        return;
//    }
//
//    for (auto paramIt = m_processedData.constBegin(); paramIt != m_processedData.constEnd(); ++paramIt) {
//        // 打印时，可以检查是否是 NaN，以便更好地区分
//        if (std::isnan(paramIt.value())) {
//            qDebug() << "  Param:" << paramIt.key() << " Value: NaN (unchecked)";
//        } else {
//            qDebug() << "  Param:" << paramIt.key() << " Value:" << paramIt.value();
//        }
//    }
//    qDebug() << "--- End of RezultInfo Processed Flat Key-Value Data ---";
//}


void RezultInfo::printOutPlateResParam(const OutPlateResParam& param)
{
    qDebug() << "--- OutPlateResParam 内容 ---";

    qDebug() << "  底座是否存在: " << param.m_PlatExist;
    qDebug() << "  底座高度: " << param.m_PlateHeight;
    qDebug() << "  底座宽度: " << param.m_PlateWid;
    qDebug() << "  底座上斜边长度: " << param.m_PlateHypUpLen;
    qDebug() << "  底座下斜边长度: " << param.m_PlateHypDownLen;
    qDebug() << "  底座瑕疵区域个数: " << param.m_PlateErrNum;
    qDebug() << "  底座压伤区域个数: " << param.m_PlateCrushNum;
    qDebug() << "  引线左引脚是否存在: " << param.m_PlatePinLeftExist;
    qDebug() << "  引线右引脚是否存在: " << param.m_PlatePinRightExist;
    qDebug() << "  引线左引脚高度: " << param.m_PlatePinLeftHeight;
    qDebug() << "  引线右引脚高度: " << param.m_PlatePinRightHeight;
    qDebug() << "  引线左引脚宽度: " << param.m_PlatePinLeftWid;
    qDebug() << "  引线右引脚宽度: " << param.m_PlatePinRightWid;
    qDebug() << "  引线左引脚角度: " << param.m_PlatePinLeftAngle;
    qDebug() << "  引线右引脚角度: " << param.m_PlatePinRightAngle;
    qDebug() << "  引线左引脚弯脚: " << param.m_PlatePinLeftBendAngle;
    qDebug() << "  引线右引脚弯脚: " << param.m_PlatePinRightBendAngle;
    qDebug() << "  引线左引脚平行度: " << param.m_PlatePinLeftParalAngle;
    qDebug() << "  引线右引脚平行度: " << param.m_PlatePinRightParalAngle;
    qDebug() << "  引线左引脚超底座长度: " << param.m_PlatePinLeftExceLen;
    qDebug() << "  引线右引脚超底座长度: " << param.m_PlatePinRightExceLen;
    qDebug() << "  引线总长度: " << param.m_PlatePinTotalLen;

    qDebug() << "--- 打印结束 ---";
}

void RezultInfo::printProcessedData() const
{
    qDebug() << "--- RezultInfo ProcessedParam 数据 ---";

    if (m_processedData.isEmpty()) {
        qDebug() << "无处理数据。";
        return;
    }

    for (const auto& param : m_processedData) {
        QString valueStr = param.value.isValid() ? param.value.toString() : "无效";
        qDebug() << "原名:" << param.originalName
            << " 映射变量:" << param.mappedVariable
            << " 检测:" << (param.isChecked ? "是" : "否")
            << " 值:" << valueStr
            << " 补偿:" << param.compensationValue;
    }

    qDebug() << "--- End ---";
}

void RezultInfo::updateProcessedData(Parameters* rangePara)
{

    processRangeParameters(rangePara);
#ifdef USE_MAIN_WINDOW_CAPACITY
    return;
#else
    emit UpdateParameters(*rangePara);
#endif // USE_MAIN_WINDOW_CAPACITY


}

void RezultInfo::updatePaintData(Parameters* rangePara)
{
    
}

float RezultInfo::getAdjustedLowerThreshold(const QString& key) const
{
    for (const auto& param : m_processedData) {
        if (param.mappedVariable == key && param.isChecked) {
            bool ok = false;
            float raw = param.value.toFloat(&ok);
            if (ok)
                return raw - static_cast<float>(param.compensationValue);  // 补偿减去
        }
    }
    return std::numeric_limits<float>::quiet_NaN();
}


const ProcessedParam* RezultInfo::getProcessedParam(const QString& mappedKey) const {
    for (const auto& p : m_processedData) {
        if (p.mappedVariable == mappedKey) return &p;
    }
    return nullptr;
}

float RezultInfo::getAdjustedUpperThreshold(const QString& key) const
{
    for (const auto& param : m_processedData) {
        if (param.mappedVariable == key && param.isChecked) {
            bool ok = false;
            float raw = param.value.toFloat(&ok);
            if (ok)
                return raw + static_cast<float>(param.compensationValue);  // 补偿加上
        }
    }
    return std::numeric_limits<float>::quiet_NaN();
}


void RezultInfo::printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange)
{

}

int RezultInfo::judge_stamp(const  OutStampResParam &ret)
{
    return 0;
}

int RezultInfo::judge_plate(const  OutPlateResParam &ret)
{
    return 0;
}

 int RezultInfo::judge_lift(const OutLiftResParam &ret)
{
     return 0;
}

int RezultInfo::judge_abut(const OutAbutResParam  &ret)
{
    return 0;
}


int RezultInfo::judge_pin(const OutPinParam& ret)
{
    return 0;
}

int RezultInfo::judge_top(const OutTopParam& ret)
{
    return 0;
}

int RezultInfo::judge_side(const OutSideParam& ret)
{
    return 0;
}

int RezultInfo::judge_stamp_min(const OutStampResParam& ret)
{
    return 0;
}

void RezultInfo::printScore()
{
    //for (int i = 0; i < score.size(); i++)
    //{
    //    std::cout << "字符 " << i << "==" << score[i];
    //}
}
void RezultInfo::updatefix(double fix)
{
    this->fix = fix;
}

