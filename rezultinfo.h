#ifndef REZULTINFO_H
#define REZULTINFO_H
#include <QObject>
#include "fileoperator.h"
#include "rangeclass.h"
#include "limits.h"

class RezultInfo : public QObject
{
    Q_OBJECT
public:
    explicit RezultInfo(RangeParameters *rangepara, QObject *parent = nullptr);

    const QMap<QString, float>& getProcessedData() const;

    void printProcessedData() const;

private:
    QMap<QString, float> m_processedData;

    void processRangeParameters(RangeParameters *rangePara);
};

#endif // REZULTINFO_H


