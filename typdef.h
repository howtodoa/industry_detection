#ifndef TYPDEF_H
#define TYPDEF_H

#define ADAPTATEION
//#define QIMAGE
#define FOURBRADER

#include <thread>
#include <opencv2/opencv.hpp>
#include "Mz_LogConn.h"
#include <QString>
#include <CapacitanceProgram.h>
#include <QVariant>
#include "BraidedTape.h"
#include "Api_FlowerPinDetection.h"
#include "MyStl.h"
#include <QtGlobal>
#include <QImage.h>

#include "Api_Welding.h"

extern std::shared_ptr<WeldingDetector> g_detector;
extern std::mutex g_detector_mutex;

struct  OutStampResParam;

struct ImageQueuePack {
    std::mutex mutex;
    std::condition_variable cond;
    MyDeque<std::shared_ptr<cv::Mat>> queue;
    std::atomic<bool> process_flag = false;

    std::mutex mutex_red;
    std::condition_variable cond_red;
    MyDeque<std::shared_ptr<cv::Mat>> queue_red;
    std::atomic<bool> red_process_flag = false;

    std::atomic<bool> stop_flag=false;
};

struct SaveData {
    std::shared_ptr<cv::Mat> imagePtr;
	QImage image;
    std::string savePath_OK;
     std::string savePath_NG;
     std::string savePath_Pre;
    std::string work_path;
    QString info="";
};

struct SaveQueue {
    std::deque<SaveData> queue;
    std::mutex mutex;
    std::condition_variable cond;
    std::atomic<bool> stopFlag = false;
};

struct DebugInfo{
     std::atomic<double> scaleFactor=0.01;
     std::atomic<int> saveflag = 0;
     float Angle = 0;
     double fix;
     bool EmptyIsOK = false;
     bool Shield = false;
     QString PicDir;
};
struct ProcessedParam
{
    QString originalName;      // 原始 JSON 中的参数名（如“引脚到底座距离检测(上限)”）
    QString mappedVariable;    // 映射变量名（如 "m_disLiftUpper"）
    QVariant value;            // 参数值（保持原始类型：float/int/bool）
    bool isChecked;            // 是否检测
    double compensationValue;  // 补偿值
};

struct PaintDataItem {
    QString label;    // 中文名称
    bool check;       // 是否检测
    QString value;    // 检测值
    int result;       // 检测结果：1 通过，0 不通过
    double offset;
    int64_t count=0;
    double scale = 1.00;
};

struct DetectInfo {
    int ret;                // 检测结果，0 表示 OK，其他表示 NG
    std::string timeStr;    // 算法耗时字符串， "32ms"
	QVector<PaintDataItem> paintDataSnapshot; // 检测数据快照
	QStringList errmsg;   // 错误信息列表
};

namespace GlobalLog {
    extern Mz_Log::COperation logger;
}

namespace LearnPara{
    extern InStampParam inParam1;
    extern InStampParam inParam2;
    extern InTopParam inParam3;
    extern InSideParam inParam4;
    extern InPinParam inParam5;
	extern InAbutParam inParam6;
    extern InFlowerPinParam inParam7;
    extern InFlowerPinParam inParam8;
    extern InLookPinParam inParam9;
    extern int inNum;//学习的字符数量
   // extern ScoreVector;
}


#define LOG_DEBUG(logger, msg) logger.Mz_AddLogEx(msg, _T(__FILE__), __LINE__)

struct SystemPara
{
public:
    static QString VERSION;
    static QString OK_DIR;
    static QString NG_DIR;
    static QString LOG_DIR;
    static QString DATA_DIR;
    static QString ROOT_DIR;
    static QString CAMERAL_DIR;
    static QString GLOBAL_DIR;
	static QString OTHER_DIR;
};

struct Camerinfo
{
public:
    QString name;
    QString ip;
    int pointNumber;
    QString path;
    QString check;
    QString mapping;
    double scaleFactor;
    int  saveflag=0;
    int rounte=0;
    float Angle=0;
    double fix;
    bool EmptyIsOK=false;
    bool Shield=false;
};


struct GlobalPara {

    enum IoType {
        VC3000,
        VC3000H,
        PCI1230
    };

    enum CameraType {
        HIKVISION = 1,
        DHUA = 2
    };

    enum Envirment {
        LocationEn = 1,
        IPCEn = 2
    };

    static IoType ioType;
    static CameraType cameraType;
    static Envirment envirment;
    static int RunPoint;
    static std::atomic<bool> changed;
    static std::atomic<bool> Stagnant;
    static QString DeviceId;
    static std::atomic<bool> cheatFlag;
    static std::atomic<bool> pulse;
	static int MergePointNum;
    static int MergePoint;
    static int Light1;
    static int Light2;
    static int Light3;
    static int Light4;
	static int FontSize;
	static int FLOWER_POS_LENGTH;
    static int FLOWER_NEG_LENGTH;
    static int TimeOut;
	static int LearnCount;
};

extern QHash<QString, MyDeque<int>> MergePointVec;
extern std::mutex g_mutex;
extern std::condition_variable g_cv;

extern std::atomic<int> GateStatus;

struct CamParamControll
{
    int width;
    int height;
    int offsetx;
    int offsety;
    float gain;
    float exposure;
    float gama;
    int fps;
};

struct SimpleParam {
    QString name;
    double value;
};

struct ExpandParam 
{
    double self_learn = 0.0f;
	bool scale_enable = true;
};


constexpr double UNIFY_UNSET_VALUE = -9999.0;

struct UnifyParam
{
    double upperLimit;
    double lowerLimit;
    double upfix;
    double lowfix;
    bool need_value;    // true: 布尔检测 (BOOL mode); false: 范围检测 (RANGE mode)
    bool check;         // 是否启用检测
    bool visible;       // 是否可见/在UI显示
    double value;       // 范围模式下的标定值/基准值；布尔模式下期望的布尔结果值 (1.0 = true)
    QString label;      // 中文名称 (映射变量)
    QString unit;       // 单位
    int result;        // 检测结果：1 通过，0 不通过
    int64_t count;      // 总检测次数
    int64_t ng_count;   // NG 次数
    double scaleFactor; // 缩放因子
    double leranValue;  // 学习补偿值
    QVariant extraData; // 额外数据字段，灵活存储其他信息  
    double accumulate;
    ExpandParam  expandParam; // 扩展参数结构体

    UnifyParam()
        : upperLimit(UNIFY_UNSET_VALUE),
        lowerLimit(UNIFY_UNSET_VALUE),
        upfix(0.0),
        lowfix(0.0),
        need_value(true),
        check(true),
        visible(true),
        value(0),
        label(""),
        unit(""),
        result(0),
        count(0),
        ng_count(0),
        scaleFactor(1.0),
        leranValue(0),
		accumulate(0.0),
        extraData(0.0)
    {
    }
    QString toString() const
    {
        QVariantMap map;
        map["label"] = label;
        map["unit"] = unit;
        map["check"] = check;
        map["visible"] = visible;
        map["need_value(IsBool)"] = need_value;
        map["value(Base/Expected)"] = value;

        // 数值范围和补偿
        map["upperLimit"] = upperLimit;
        map["lowerLimit"] = lowerLimit;
        map["upfix"] = upfix;
        map["lowfix"] = lowfix;

        // 统计和缩放
        map["count"] = (qint64)count;
        map["ng_count"] = (qint64)ng_count;
        map["scaleFactor"] = scaleFactor;

        QString s = QString("UnifyParam for '%1' (%2):\n").arg(label, unit);

        // 遍历 Map，逐行打印键值对
        for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
            s += QString("  %1: %2\n").arg(it.key(), it.value().toString());
        }

        return s;
    }

    static double getRandomDouble(double min, double max) {
        if (min >= max) return min;
        // 使用 rand() 和 RAND_MAX 实现随机数
        return min + (static_cast<double>(rand()) / RAND_MAX) * (max - min);
    }

    void toLogString() const
    {
        // 1. 构建详细的日志消息 (使用 toDebugString() 的逻辑)
        QString logMsg;

        logMsg = QString("--- UnifyParam DEBUG DUMP for '%1' (%2) ---\n")
            .arg(label, unit);

        logMsg += QString("  [Config] Mode: %1, Check: %2, Visible: %3, Result: %4\n")
            .arg(need_value ? "BOOL" : "RANGE")
            .arg(check ? "ON" : "OFF")
            .arg(visible ? "YES" : "NO")
            .arg(result == 1 ? "PASS" : (result == 0 ? "NG" : "N/A"));

        logMsg += QString("  [Value] Current: %1, Learn: %2, Scale: %3\n")
            .arg(value)
            .arg(leranValue)
            .arg(scaleFactor);

        // 注意这里使用了 %lld 打印 64 位整数
        logMsg += QString("  [Stats] TotalCount: %1, NG_Count: %2\n")
            .arg(static_cast<qint64>(count))
            .arg(static_cast<qint64>(ng_count));

        logMsg += QString("  [Range] Limits: [%1 - %2, %3 + %4]\n")
            .arg(lowerLimit)
            .arg(lowfix)
            .arg(upperLimit)
            .arg(upfix);

        logMsg += QString("  [Extra] Data Type: %1\n").arg(extraData.typeName());

        logMsg += "--- END DEBUG DUMP ---";

        // 2. 调用您的 LOG_DEBUG 宏进行打印
        // 核心打印逻辑：
        LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
    }

    void applyCorrection()
    {
        double currentValue = this->value;
        double correctedValue = currentValue;
        const double MaxOffsetPercent = 0.1; // 10%

        // ----------------------------------------------------
        // I. 检查是否在下限补偿范围内: [下限 - lowfix, 下限 + lowfix)
        // ----------------------------------------------------
        if (currentValue >= (this->lowerLimit - this->lowfix) &&
            currentValue < (this->lowerLimit + this->lowfix))
        {
            // 处于下限补偿区，将值拉回到 [下限, 下限 + 10%*下限] 范围内
            double offset = this->lowerLimit * MaxOffsetPercent * getRandomDouble(0.0, 1.0);
            correctedValue = this->lowerLimit + offset;
            qDebug() << "    -> INFO: Value in Low-Fix range. Corrected from" << currentValue << "to" << correctedValue;
        }
        // ----------------------------------------------------
        // II. 检查是否在上限补偿范围内: (上限 - upfix, 上限 + upfix]
        // ----------------------------------------------------
        else if (currentValue > (this->upperLimit - this->upfix) &&
            currentValue <= (this->upperLimit + this->upfix))
        {
            // 处于上限补偿区，将值拉回到 [上限 - 10%*上限, 上限] 范围内
            double offset = this->upperLimit * MaxOffsetPercent * getRandomDouble(0.0, 1.0);
            correctedValue = this->upperLimit - offset;
            qDebug() << "    -> INFO: Value in Up-Fix range. Corrected from" << currentValue << "to" << correctedValue;
        }

        // 如果值被矫正，更新 this->value
        if (correctedValue != currentValue) {
            this->value = correctedValue;
        }
    }

    void change_value()
    {
        this->value = UNIFY_UNSET_VALUE;
        this->need_value = false;
		result = -1;
    }

    bool checkBool()
    {
        // 获取实测布尔值 (this->value > 0.5)
        bool actualBool = (this->value > 0.5);

        // 获取期望布尔值
        bool expectedBool = this->need_value;

        // 结果即为两者是否相等
        bool result = (actualBool == expectedBool);

        // 打印调试信息
        qDebug().nospace() << "  -> Judge Result: " << (result ? "PASS" : "FAIL")
            << " (Actual: " << actualBool << ", Expected: " << expectedBool << ")";

        return result;
    }
    bool checkRange()
    {
        // 1. 核心判定：判断实测值是否在 [下限 - lowfix, 上限 + upfix] 范围内（包含补偿区）
        bool isTotalPass = (this->upperLimit == UNIFY_UNSET_VALUE ||
            this->value <= (this->upperLimit + this->upfix)) &&
            (this->lowerLimit == UNIFY_UNSET_VALUE ||
                this->value >= (this->lowerLimit - this->lowfix));

        // 2. 如果通过了总判定，执行矫正操作 (如果实测值在补偿区内，this->value 会被修改)
        if (isTotalPass)
        {
            this->applyCorrection();
            qDebug().nospace() << "  -> PASS: Value " << this->value << " is within total range (corrected).";
            return true;
        }
        else
        {
            // 绝对 NG
            qDebug().nospace() << "  -> FAIL: Value " << this->value << " is OUTSIDE total range.";
            return false;
        }
    }
    bool checkQListRange()
    {
        // 1. 检查 extraData 是否是有效的 QVariantList
        if (!this->extraData.isValid() || this->extraData.type() != QVariant::List) {
            qDebug() << "  -> QList Check: extraData is not a valid QVariantList or is empty. Passing by default.";
            return true;
        }

        // 2. 检查上下限是否设置
        bool lowerIsUnset = qFuzzyCompare(this->lowerLimit, UNIFY_UNSET_VALUE);
        bool upperIsUnset = qFuzzyCompare(this->upperLimit, UNIFY_UNSET_VALUE);

        // 如果上下限均未设置，则跳过数组范围检测（但数组存在）
        if (lowerIsUnset && upperIsUnset) {
            qDebug() << "  -> QList Check: Range limits are unset. Passing by default.";
            return true;
        }

        // 3. 计算包含补偿值的实际有效范围
        // 补偿值 (lowfix, upfix) 对数组的每一个数都生效
        const double infinity = std::numeric_limits<double>::infinity();

        // effectiveLower = 下限 - 下限补偿值
        const double effectiveLower = lowerIsUnset ? -infinity : (this->lowerLimit - this->lowfix);

        // effectiveUpper = 上限 + 上限补偿值
        const double effectiveUpper = upperIsUnset ? infinity : (this->upperLimit + this->upfix);

        const QVariantList list = this->extraData.toList();

        // 4. 遍历并比较数组中的每个元素
        for (int i = 0; i < list.size(); ++i)
        {
            // 从 QVariant 中取出数值（假设存储的是 float/double）
            double value = list.at(i).toDouble();

            // 【核心判断】：检查元素是否在有效范围内
            bool isPassed = (value >= effectiveLower) && (value <= effectiveUpper);

            if (!isPassed)
            {
                qDebug().nospace() << "  -> QList Check FAIL at index " << i
                    << ": Value " << value
                    << " is outside [" << effectiveLower << ", " << effectiveUpper << "]";

                // 只要有一个元素不通过，就返回 false
                return false;
            }
        }

        qDebug() << "  -> QList Check PASS: All elements are within range.";
        return true; // 所有元素都通过
    }

    void Accumulate()
    {
        double valueToAccumulate = 0.0;

        // ----------------------------------------------------
        // I. 检查 extraData 是否是 QVariantList
        // ----------------------------------------------------
        if (this->extraData.isValid() && this->extraData.typeId() == QMetaType::QVariantList)
        {
            const QVariantList list = this->extraData.toList();
            if (!list.empty())
            {
                double sum = 0.0;
                int numericalCount = 0;

                // 遍历列表，计算所有可转换数值的总和和个数
                for (const QVariant& var : list)
                {
                    if (var.canConvert<double>())
                    {
                        sum += var.toDouble();
                        numericalCount++;
                    }
                }

                if (numericalCount > 0)
                {
                    // 如果是列表，计算平均值作为本次累加的代表值
                    valueToAccumulate = sum / numericalCount;
					this->accumulate += valueToAccumulate;
                    qDebug() << QString("Accumulate for '%1': Used Mean of extraData List (%2 items): %3")
                        .arg(this->label)
                        .arg(numericalCount)
                        .arg(valueToAccumulate);
                }
                else {
                    // 列表不含数值，使用默认值 0.0，但本次运行仍会被统计
                    qDebug() << QString("Accumulate for '%1': extraData List found, but contains no numerical data. Using 0.0.").arg(this->label);
                }
            }
        }
        else
        {
            // ----------------------------------------------------
            // II. extraData 不存在或不是列表，使用 this->value
            // ----------------------------------------------------

            // 假设 this->value 存储的是本次运行的结果值
            this->accumulate += this->value;
            qDebug() << QString("Accumulate for '%1': Used current single value: %2").arg(this->label).arg(valueToAccumulate);
        }
    }
    void applyLearningLimits()
    {

        double mean = this->accumulate / static_cast<double>(GlobalPara::LearnCount);

        // 新的上限 = 基准均值 + 补偿量
        this->upperLimit = mean + this->leranValue;

        // 新的下限 = 基准均值 - 补偿量
        this->lowerLimit = mean - this->leranValue;

		this->accumulate = 0.0; // 重置累加值以备下次学习使用
    }
};

// 必须在结构体外部实现
inline QDebug operator<<(QDebug dbg, const UnifyParam& param)
{
    // 最佳实践：保存并恢复 QDebug 的状态
    QDebugStateSaver saver(dbg);

    // 启用紧凑模式，让输出更整洁
    dbg.nospace() << "UnifyParam(\"" << param.label << "\") {\n";

    // 1. 模式/状态
    dbg.nospace() << "  Mode: " << (param.need_value ? "BOOL" : "RANGE")
        << ", Check Enabled: " << (param.check ? "Yes" : "No")
        << ", Visible: " << (param.visible ? "Yes" : "No") << "\n";

    // 2. 核心值/结果
    dbg.nospace() << "  Value: " << param.value
        << " " << param.unit
        << ", Result: " << (param.result == 1 ? "PASS" : (param.result == 0 ? "NG" : "N/A"))
        << "\n";

    // 3. 范围和补偿
    dbg.nospace() << "  Range: [" << param.lowerLimit << " - " << param.lowfix
        << ", " << param.upperLimit << " + " << param.upfix << "]\n";

    // 4. 统计
    // 注意：qint64 转换是正确的做法
    dbg.nospace() << "  Stats: Total=" << (qint64)param.count
        << ", NG=" << (qint64)param.ng_count
        << ", ScaleFactor=" << param.scaleFactor << "\n";

    // 5. 其他
    dbg.nospace() << "  Extra: Learn=" << param.leranValue
        << ", ExtraDataType=" << param.extraData.typeName()
        // 确保最后的 "}" 也在 nospace() 的作用下
        << "}\n";

    // saver 析构时会恢复 dbg 的状态

    return dbg;
}

using AllUnifyParams = QMap<QString, UnifyParam>;
#endif // TYPDEF_H
