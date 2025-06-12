#include "CameralClass.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

CameralClass::CameralClass(QObject *parent)
    : QObject(parent)
{
}

CameralClass::CameralClass(QString cameralpath, QObject *parent)
    : QObject(parent), m_cameralPath(cameralpath)
{
    loadParams();
}

bool CameralClass::loadParams()
{
    if (m_cameralPath.isEmpty()) {
        qWarning() << "CameralClass::loadParams: m_cameralPath is empty. Cannot load parameters.";
        return false;
    }

    QFile file(m_cameralPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "CameralClass::loadParams: Could not open file for reading:" << m_cameralPath
                   << "Error:" << file.errorString();
        return false;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull()) {
        qWarning() << "CameralClass::loadParams: Failed to create JSON document from data. Invalid JSON.";
        return false;
    }
    if (!doc.isObject()) {
        qWarning() << "CameralClass::loadParams: JSON document is not an object.";
        return false;
    }

    QJsonObject rootObj = doc.object();
    QVariantMap rootMap = rootObj.toVariantMap();

    const QString topLevelKey = "相机参数"; // Or whatever your camera JSON's root key is

    if (rootMap.contains(topLevelKey) && rootMap.value(topLevelKey).type() == QVariant::Map) {
        m_parameters = Parameters(rootMap.value(topLevelKey).toMap());
        qDebug() << "CameralClass::loadParams: Parameters loaded successfully from" << m_cameralPath;
        return true;
    } else {
        qWarning() << "CameralClass::loadParams: JSON does not contain '" << topLevelKey << "' key or it's not an object.";
        return false;
    }
}

bool CameralClass::saveParams()
{
    if (m_cameralPath.isEmpty()) {
        qWarning() << "CameralClass::saveParams: m_cameralPath is empty. Cannot save parameters.";
        return false;
    }

    QVariantMap rootMap;
    const QString topLevelKey = "相机参数";
    rootMap[topLevelKey] = m_parameters.toVariantMap();

    QJsonObject rootObj = QJsonObject::fromVariantMap(rootMap);
    QJsonDocument doc(rootObj);

    QFile file(m_cameralPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qWarning() << "CameralClass::saveParams: Could not open file for writing:" << m_cameralPath
                   << "Error:" << file.errorString();
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    qDebug() << "CameralClass::saveParams: Parameters saved successfully to" << m_cameralPath;
    return true;
}

const ParamDetail* CameralClass::getParamDetail(const QString& projectKey, const QString& paramName) const
{
    auto projectIt = m_parameters.detectionProjects.find(projectKey); // Changed to detectionProjects
    if (projectIt != m_parameters.detectionProjects.end()) { // Changed to detectionProjects
        auto paramIt = projectIt->params.find(paramName);
        if (paramIt != projectIt->params.end()) {
            return &paramIt.value();
        }
    }
    return nullptr;
}

void CameralClass::updateParamValue(const QString& projectKey, const QString& paramName, const QVariant& newValue)
{
    auto projectIt = m_parameters.detectionProjects.find(projectKey); // Changed to detectionProjects
    if (projectIt != m_parameters.detectionProjects.end()) { // Changed to detectionProjects
        auto paramIt = projectIt->params.find(paramName);
        if (paramIt != projectIt->params.end()) {
            paramIt.value().value = newValue;
        } else {
            qWarning() << "CameralClass::updateParamValue: Parameter" << paramName << "not found in project" << projectKey;
        }
    } else {
        qWarning() << "CameralClass::updateParamValue: Project" << projectKey << "not found.";
    }
}

void CameralClass::updateParamCheck(const QString& projectKey, const QString& paramName, bool checkValue)
{
    auto projectIt = m_parameters.detectionProjects.find(projectKey); // Changed to detectionProjects
    if (projectIt != m_parameters.detectionProjects.end()) { // Changed to detectionProjects
        auto paramIt = projectIt->params.find(paramName);
        if (paramIt != projectIt->params.end()) {
            paramIt.value().check = checkValue;
            qDebug() << "CameralClass::updateParamCheck: Updated check for" << projectKey << "/" << paramName << "to" << checkValue;
        } else {
            qWarning() << "CameralClass::updateParamCheck: Parameter" << paramName << "not found in project" << projectKey;
        }
    } else {
        qWarning() << "CameralClass::updateParamCheck: Project" << projectKey << "not found.";
    }
}
