#ifndef CAMERAL_H
#define CAMERAL_H

#include <QObject>
#include <mutex>
#include <atomic>
#include <deque>
#include "rangeclass.h"
#include "algoclass.h"
#include "cameralclass.h"
#include "public.h"
#include "rezultinfo_nayin.h"
#include "MsvDeviceLib.h"


class Cameral : public QObject
{
    Q_OBJECT
public:
    explicit Cameral(QObject *parent = nullptr);
    ~Cameral();

    QString algopath;
    QString rangepath;
    QString cameralpath;
    QString ok_path;
    QString ng_path;
    QString localpath;
    QString cameral_name;
    QString ScalePath;
    RangeClass *RC;
    AlgoClass *AC;
    CameralClass *CC;
    RezultInfo  * RI=nullptr;
    ImageQueuePack imageQueue;
    std::atomic<int64> error_count=0;
    std::atomic<int64> sum_count=0;
    std::atomic <bool> video=false;
    std::atomic <bool> local=false;
    std::atomic <bool> photo=false;
    std::atomic <bool> learn = false;
    std::atomic<bool> running = false;
    std::atomic<int> pointNumber;
    std::atomic<bool> noneDisplay=false;
    std::atomic<bool> ui_signal = false;
    std::atomic<int> ten=10;
    std::atomic<int> triggerNum = 0;
    bool m_cameraWarningShown = false;
    std::string indentify="";
    QVector<SimpleParam> ScaleArray;
    int rounte = 0;
	AllUnifyParams unifyParams;
    double scaleFactor=0.01;
    DebugInfo DI;
    Mz_CameraConn::COperation *camOp=nullptr;
};

#endif // CAMERAL_H
