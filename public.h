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
public:
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

    // 序列化为字符串
    QString toString() const {
        return QString("范围参数：\n"
                       "正极针长下限: %1\n"
                       "负极针长下限: %2\n"
                       "负极针宽下限: %3\n"
                       "针总长下限: %4\n"
                       "正针弯角度最小: %5\n"
                       "负针弯角度最小: %6\n"
                       "正极针长上限: %7\n"
                       "负极针长上限: %8\n"
                       "负极针宽上限: %9\n"
                       "针总长上限: %10\n"
                       "正针弯角度最大: %11\n"
                       "负针弯角度最大: %12")
            .arg(nePinLengthLower).arg(nePinWidthLower).arg(poPinLengthLower)
            .arg(piTotalLengthLower).arg(poBendAngleMin).arg(neBendAngleMin)
            .arg(nePinLengthUpper).arg(nePinWidthUpper).arg(poPinLengthUpper)
            .arg(piTotalLengthUpper).arg(poBendAngleMax).arg(neBendAngleMax);
    }
    bool fromString(const QString& content) {
        QStringList lines = content.split("\n", Qt::SkipEmptyParts);
        if (lines.size() < 13 || lines[0] != "范围参数：") return false;
        nePinLengthLower = lines[1].split(": ")[1].toFloat();
        nePinWidthLower = lines[2].split(": ")[1].toFloat();
        poPinLengthLower = lines[3].split(": ")[1].toFloat();
        piTotalLengthLower = lines[4].split(": ")[1].toFloat();
        poBendAngleMin = lines[5].split(": ")[1].toFloat();
        neBendAngleMin = lines[6].split(": ")[1].toFloat();
        nePinLengthUpper = lines[7].split(": ")[1].toFloat();
        nePinWidthUpper = lines[8].split(": ")[1].toFloat();
        poPinLengthUpper = lines[9].split(": ")[1].toFloat();
        piTotalLengthUpper = lines[10].split(": ")[1].toFloat();
        poBendAngleMax = lines[11].split(": ")[1].toFloat();
        neBendAngleMax = lines[12].split(": ")[1].toFloat();
        return true;
    }
};

struct AlgorithmPara
{
public:
    QString Algorithm_Name;       // 算法名称
    QVector<QString> Value_Names; // 算法参数名称
    QVector<QString> Value_Values;// 算法参数值

    // 序列化为字符串
    QString toString() const {
        return QString("算法参数：\n"
                       "算法名称: %1\n"
                       "算法参数名称: %2\n"
                       "算法参数值: %3")
            .arg(Algorithm_Name)
            .arg(Value_Names.join(";"))
            .arg(Value_Values.join(";"));
    }
    bool fromString(const QString& content) {
        QStringList lines = content.split("\n", Qt::SkipEmptyParts);
        if (lines.size() < 4 || lines[0] != "算法参数：") return false;
        Algorithm_Name = lines[1].split(": ")[1];
        Value_Names = lines[2].split(": ")[1].split(";");
        Value_Values = lines[3].split(": ")[1].split(";");
        return true;
    }
};

struct CameralPara
{
public:
    QString Cameral_Name;    // 相机名称
    QString Cameral_SN;      // 相机序列号
    QString Cameral_IP;      // 相机IP
    int     Cameral_Type;    // 相机类型
    int     Cameral_Station; // 相机工位号
    int     Cameral_AD;      // 相机AD
    int     Cameral_Gain;    // 相机增益
    int     Cameral_Exposure;// 相机曝光

    // 序列化为字符串
    QString toString() const {
        return QString("相机参数：\n"
                       "相机名称: %1\n"
                       "相机序列号: %2\n"
                       "相机IP: %3\n"
                       "相机类型: %4\n"
                       "相机工位号: %5\n"
                       "相机AD: %6\n"
                       "相机增益: %7\n"
                       "相机曝光: %8")
            .arg(Cameral_Name).arg(Cameral_SN).arg(Cameral_IP)
            .arg(Cameral_Type).arg(Cameral_Station).arg(Cameral_AD)
            .arg(Cameral_Gain).arg(Cameral_Exposure);
    }
    bool fromString(const QString& content) {
        QStringList lines = content.split("\n", Qt::SkipEmptyParts);
        if (lines.size() < 9 || lines[0] != "相机参数：") return false;
        Cameral_Name = lines[1].split(": ")[1];
        Cameral_SN = lines[2].split(": ")[1];
        Cameral_IP = lines[3].split(": ")[1];
        Cameral_Type = lines[4].split(": ")[1].toInt();
        Cameral_Station = lines[5].split(": ")[1].toInt();
        Cameral_AD = lines[6].split(": ")[1].toInt();
        Cameral_Gain = lines[7].split(": ")[1].toInt();
        Cameral_Exposure = lines[8].split(": ")[1].toInt();
        return true;
    }
};

#endif // PUBLIC_H
