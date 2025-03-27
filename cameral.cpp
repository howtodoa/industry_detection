#include "cameral.h"

Cameral::Cameral(QObject *parent) :
    QObject(parent)
{
    // 初始化 rangeParams 的默认值
    rangeParams.nePinLengthLower = 0.0f;
    rangeParams.nePinWidthLower = 0.0f;
    rangeParams.poPinLengthLower = 0.0f;
    rangeParams.piTotalLengthLower = 0.0f;
    rangeParams.poBendAngleMin = 0.0f;
    rangeParams.neBendAngleMin = 0.0f;
    rangeParams.nePinLengthUpper = 0.0f;
    rangeParams.nePinWidthUpper = 0.0f;
    rangeParams.poPinLengthUpper = 0.0f;
    rangeParams.piTotalLengthUpper = 0.0f;
    rangeParams.poBendAngleMax = 0.0f;
    rangeParams.neBendAngleMax = 0.0f;

    // 初始化 cameralParams 的默认值
    cameralParams.Cameral_Name = "";
    cameralParams.Cameral_SN = "";
    cameralParams.Cameral_IP = "";
    cameralParams.Cameral_Type = 0;
    cameralParams.Cameral_Station = 0;
    cameralParams.Cameral_AD = 0;
    cameralParams.Cameral_Gain = 0;
    cameralParams.Cameral_Exposure = 0;

    // 初始化 algoParams 的默认值
    algoParams.Algorithm_Name = "";
    algoParams.Value_Names.clear();
    algoParams.Value_Values.clear();
}

Cameral::~Cameral()
{

}
