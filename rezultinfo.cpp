#include "rezultinfo.h"
#include <QMap>
#include <QVariant>


RezultInfo::RezultInfo(RangeParameters *rangePara, QObject *parent)
    : QObject(parent)
{
    if (rangePara) {
        processRangeParameters(rangePara);
    } else {
        qWarning("RezultInfo: RangeParameters pointer is null. No data will be processed.");
    }
}

void RezultInfo::processRangeParameters(RangeParameters *rangePara)
{
    m_processedData.clear();

    for (auto projectIt = rangePara->detectionProjects.begin();
         projectIt != rangePara->detectionProjects.end();
         ++projectIt)
    {
        const QString& projectName = projectIt.key();
        const DetectionProject& detectionProject = projectIt.value();

        QMap<QString, ParamDetail> currentProjectParams;

        for (auto paramIt = detectionProject.params.begin();
             paramIt != detectionProject.params.end();
             ++paramIt)
        {
            const QString& paramName = paramIt.key();
            const ParamDetail& paramDetail = paramIt.value();

            if (paramDetail.check) {
                currentProjectParams[paramName] = paramDetail;
            }
        }

        if (!currentProjectParams.isEmpty()) {
            m_processedData[projectName] = currentProjectParams;
        }
    }
}

const QMap<QString, QMap<QString, ParamDetail>>& RezultInfo::getProcessedData() const
{
    return m_processedData;
}
