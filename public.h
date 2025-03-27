#ifndef PUBLIC_H
#define PUBLIC_H


#include "QList"
#include "QDebug"
#include "QLabel"
#include "QFont"
#include "QGridLayout"
#include "QPushButton"
#include "QScreen"
#include "QMenu"
#include "QMenuBar"
#include "QAction"
#include "QLabel"
#include "QStatusBar"
#include "QTimer"
#include "QDir"
#include "QFileDialog"
#include "QDesktopServices"
#include "QString"
#include "QVector"

struct RangePara {
    // 下限值
    float nePinLengthLower;  // 负极针长下限 (Negative Pin Length Lower)
    float nePinWidthLower;   // 负极针宽下限 (Negative Pin Width Lower)
    float poPinLengthLower;  // 正极针长下限 (Positive Pin Length Lower)
    float piTotalLengthLower; // 针总长下限 (Pin Total Length Lower)
    float poBendAngleMin;    // 正针弯角度最小 (Positive Bend Angle Minimum)
    float neBendAngleMin;    // 负针弯角度最小 (Negative Bend Angle Minimum)

    // 上限值
    float nePinLengthUpper;  // 负极针长上限 (Negative Pin Length Upper)
    float nePinWidthUpper;   // 负极针宽上限 (Negative Pin Width Upper)
    float poPinLengthUpper;  // 正极针长上限 (Positive Pin Length Upper)
    float piTotalLengthUpper; // 针总长上限 (Pin Total Length Upper)
    float poBendAngleMax;    // 正针弯角度最大 (Positive Bend Angle Maximum)
    float neBendAngleMax;    // 负针弯角度最大 (Negative Bend Angle Maximum)
};

struct AlgorithmPara
{
    QString Algorithm_Name; //算法名称
    QVector<QString> Value_Names; //算法参数名称
    QVector<QString> Value_Values;  //算法参数值
};

struct CameralPara
{
    QString Cameral_Name; //相机名称
    QString Cameral_SN; //相机席列号
    QString Cameral_IP; //相机IP
    int     Cameral_Type; //相机类型
    int     Cameral_Station; //相机丁位号
    int     Cameral_AD; //相机AD
    int     Cameral_Gain; //相机增益
    int     Cameral_Exposure; //相机曝光
};

#endif // PUBLIC_H
