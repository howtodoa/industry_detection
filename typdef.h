#ifndef TYPDEF_H
#define TYPDEF_H

#include <thread>
#include <opencv2/opencv.hpp>
#include "Mz_LogConn.h"
#include <QString>
#include <CapacitanceProgram.h>
#include <QVariant>
#include "BraidedTape.h"


struct  OutStampResParam;

struct ImageQueuePack {
    std::mutex mutex;
    std::condition_variable cond;
    std::deque<std::shared_ptr<cv::Mat>> queue;
    std::atomic<bool> stop_flag=false;
};

struct SaveData {
    std::shared_ptr<cv::Mat> imagePtr;
    std::string savePath_OK;
     std::string savePath_NG;
     std::string savePath_Pre;
    std::string work_path;
    std::string info="";
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
};

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


#endif // TYPDEF_H
