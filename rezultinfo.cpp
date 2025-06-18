#include "rezultinfo.h"
#include <QMap>
#include <QVariant>



RezultInfo::RezultInfo(RangeParameters *rangepara, QObject *parent)
    : QObject(parent)
{
    processRangeParameters(rangepara);
}

const QMap<QString, float>& RezultInfo::getProcessedData() const
{
    return m_processedData;
}

void RezultInfo::processRangeParameters(RangeParameters *rangePara)
{
    if (!rangePara) {
        qWarning() << "RezultInfo: RangeParameters pointer is null during processing. m_processedData will be empty.";
        m_processedData.clear();
        return;
    }

    m_processedData.clear();

    for (auto projectIt = rangePara->detectionProjects.constBegin();
         projectIt != rangePara->detectionProjects.constEnd();
         ++projectIt)
    {
        const DetectionProject& project = projectIt.value();

        for (auto paramIt = project.params.constBegin();
             paramIt != project.params.constEnd();
             ++paramIt)
        {
            QString paramName = paramIt.key();
            const ParamDetail& detail = paramIt.value();

            if (detail.check) {
                // 如果 check 为 true，保存实际的浮点值
                m_processedData.insert(paramName, detail.value.toFloat());
            } else {
                // 如果 check 为 false，保存 NaN
                m_processedData.insert(paramName, std::numeric_limits<float>::quiet_NaN());
            }
        }
    }
    qDebug() << "RezultInfo: Parameters processed. Total parameters stored (flat, with NaN for unchecked):" << m_processedData.size();
}

void RezultInfo::printProcessedData() const
{
    qDebug() << "--- RezultInfo Processed Flat Key-Value Data (float/NaN) ---";
    if (m_processedData.isEmpty()) {
        qDebug() << "No processed data available.";
        return;
    }

    for (auto paramIt = m_processedData.constBegin(); paramIt != m_processedData.constEnd(); ++paramIt) {
        // 打印时，可以检查是否是 NaN，以便更好地区分
        if (std::isnan(paramIt.value())) {
            qDebug() << "  Param:" << paramIt.key() << " Value: NaN (unchecked)";
        } else {
            qDebug() << "  Param:" << paramIt.key() << " Value:" << paramIt.value();
        }
    }
    qDebug() << "--- End of RezultInfo Processed Flat Key-Value Data ---";
}
