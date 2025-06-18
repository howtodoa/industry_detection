#ifndef CAMERAL_H
#define CAMERAL_H

#include <QObject>
#include <mutex>
#include <atomic>
#include <QString>
#include "rangeclass.h"
#include "algoclass.h"
#include "cameralclass.h"
#include "public.h"
#include "rezultinfo_nayin.h"
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
    RangeClass *RC;
    AlgoClass *AC;
    CameralClass *CC;
    RezultInfo_NaYin * NY=nullptr;
    std::mutex mutex;
    std::atomic<bool> photoflag;
};

#endif // CAMERAL_H
