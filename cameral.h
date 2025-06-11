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

class Cameral : public QObject
{
    Q_OBJECT
public:
    explicit Cameral(QObject *parent = nullptr);
    ~Cameral();

    QString algopath;
    QString rangepath;
    QString cameralpath;
    RangeClass *RC;
    AlgoClass *AC;
    CameralClass *CC;
    std::mutex mutex;
    std::atomic<bool> photoflag;
};

#endif // CAMERAL_H
