#include "rezultinfo_abut.h"

RezultInfo_Abut::RezultInfo_Abut(Parameters *rangepara, QObject *parent)
    : RezultInfo(rangepara, parent) // 调用基类 RezultInfo 的构造函数，它会负责加载和处理参数
{

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
    //const QMap<QString, float>& thresholds = this->getProcessedData();

    //auto getParamFloatValue = [&](const QString& key) -> float {
    //    return thresholds.contains(key) ? thresholds.value(key) : std::numeric_limits<float>::quiet_NaN();
    //};

    //// 所有检测项是否有失败
    //bool hasFail = false;

    //// 所有参数值统一初始化
    //float isHaveProdExpectedFloat = getParamFloatValue("isHaveProd");
    //float isHavePpinExpectedFloat = getParamFloatValue("isHavePpin");
    //float isHaveNpinExpectedFloat = getParamFloatValue("isHaveNpin");
    //float isHaveBplnExpectedFloat = getParamFloatValue("isHaveBpln");

    //float Pin_CUpper = getParamFloatValue("Pin_CUpper");
    //float Pin_CLower = getParamFloatValue("Pin_CLower");
    //float shuyao_widthUpper = getParamFloatValue("shuyao_widthUpper");
    //float shuyao_widthLower = getParamFloatValue("shuyao_widthLower");
    //float plate_widthUpper = getParamFloatValue("plate_widthUpper");
    //float plate_widthLower = getParamFloatValue("plate_widthLower");
    //float p_pin_over_plnUpper = getParamFloatValue("p_pin_over_plnUpper");
    //float p_pin_over_plnLower = getParamFloatValue("p_pin_over_plnLower");
    //float n_pin_over_plnUpper = getParamFloatValue("n_pin_over_plnUpper");
    //float n_pin_over_plnLower = getParamFloatValue("n_pin_over_plnLower");
    //float p_pin_HUpper = getParamFloatValue("p_pin_HUpper");
    //float p_pin_HLower = getParamFloatValue("p_pin_HLower");
    //float n_pin_HUpper = getParamFloatValue("n_pin_HUpper");
    //float n_pin_HLower = getParamFloatValue("n_pin_HLower");
    //float p_n_height_diffUpper = getParamFloatValue("p_n_height_diffUpper");
    //float p_n_height_diffLower = getParamFloatValue("p_n_height_diffLower");
    //float p_pin_AngleUpper = getParamFloatValue("p_pin_AngleUpper");
    //float p_pin_AngleLower = getParamFloatValue("p_pin_AngleLower");
    //float n_pin_AngleUpper = getParamFloatValue("n_pin_AngleUpper");
    //float n_pin_AngleLower = getParamFloatValue("n_pin_AngleLower");
    //float p_pin_McUpper = getParamFloatValue("p_pin_McUpper");
    //float n_pin_McUpper = getParamFloatValue("n_pin_McUpper");
    //float b_pln_McUpper = getParamFloatValue("b_pln_McUpper");

    //// --- 辅助函数 ---
    //auto checkRange = [&](const QString& name, float val, float low, float up) {
    //    bool out = false, configValid = true;
    //    float failThresh = 0.0f;
    //    bool isUpper = false;

    //    if (!std::isnan(low) && !std::isnan(up) && low > up) {
    //        configValid = false;
    //        qWarning() << "WARNING:" << name << "下限 > 上限，配置无效，视为FAIL";
    //        out = true; failThresh = up; isUpper = true;
    //    }

    //    if (configValid) {
    //        if (!std::isnan(low) && val < low) { out = true; failThresh = low; isUpper = false; }
    //        if (!std::isnan(up) && val > up) { out = true; failThresh = up; isUpper = true; }
    //    }

    //    this->printCheckInfo(name, val, failThresh, isUpper, out);
    //    if (out) hasFail = true;
    //};

    //auto checkUpper = [&](const QString& name, float val, float upper) {
    //    bool out = false;
    //    if (!std::isnan(upper)) out = (val > upper);
    //    this->printCheckInfo(name, val, upper, true, out);
    //    if (out) hasFail = true;
    //};

    //auto checkBool = [&](const QString& name, float expected, bool actual) {
    //    bool exp = (expected == 1.0f);
    //    bool out = (actual != exp);
    //    this->printCheckInfo(name, static_cast<float>(actual), expected, false, out);
    //    if (out) hasFail = true;
    //};

    //// --- 检测流程 ---

    //if (!std::isnan(isHaveProdExpectedFloat)) checkBool("是否有料检测", isHaveProdExpectedFloat, ret.isHaveProd);
    //else qDebug() << "INFO: 是否有料检测 skipped.";

    //if (!std::isnan(isHavePpinExpectedFloat)) checkBool("是否有左引脚检测", isHavePpinExpectedFloat, ret.isHavePpin);
    //else qDebug() << "INFO: 是否有左引脚检测 skipped.";

    //if (!std::isnan(isHaveNpinExpectedFloat)) checkBool("是否有右引脚检测", isHaveNpinExpectedFloat, ret.isHaveNpin);
    //else qDebug() << "INFO: 是否有右引脚检测 skipped.";

    //if (!std::isnan(isHaveBplnExpectedFloat)) checkBool("是否有座板检测", isHaveBplnExpectedFloat, ret.isHaveBpln);
    //else qDebug() << "INFO: 是否有座板检测 skipped.";

    //checkRange("引脚总长检测", ret.Pin_C, Pin_CLower, Pin_CUpper);
    //checkRange("束腰宽度检测", ret.shuyao_width, shuyao_widthLower, shuyao_widthUpper);
    //checkRange("底座宽度检测", ret.plate_width, plate_widthLower, plate_widthUpper);
    //checkRange("左引脚超板检测", ret.p_pin_over_pln, p_pin_over_plnLower, p_pin_over_plnUpper);
    //checkRange("右引脚超板检测", ret.n_pin_over_pln, n_pin_over_plnLower, n_pin_over_plnUpper);
    //checkRange("左引脚翘脚检测", ret.p_pin_H, p_pin_HLower, p_pin_HUpper);
    //checkRange("右引脚翘脚检测", ret.n_pin_H, n_pin_HLower, n_pin_HUpper);
    //checkRange("左右引脚高度差检测", ret.p_n_height_diff, p_n_height_diffLower, p_n_height_diffUpper);
    //checkRange("左引脚角度检测", ret.p_pin_Angle, p_pin_AngleLower, p_pin_AngleUpper);
    //checkRange("右引脚角度检测", ret.n_pin_Angle, n_pin_AngleLower, n_pin_AngleUpper);

    //checkUpper("左引脚毛刺检测", ret.p_pin_Mc, p_pin_McUpper);
    //checkUpper("右引脚毛刺检测", ret.n_pin_Mc, n_pin_McUpper);
    //checkUpper("基座毛刺检测", ret.b_pln_Mc, b_pln_McUpper);

    //if (hasFail) {
    //    qDebug() << "--- Some Abut checks failed ---";
    //    return -1;
    //}

    //qDebug() << "--- All Abut checks passed ---";
    //return 0;
return 0;
}
