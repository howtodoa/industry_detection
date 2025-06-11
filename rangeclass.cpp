#include "fileoperator.h"
#include "rangeclass.h"

#include <QObject>
#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

RangeClass::RangeClass(QObject *parent)
    : QObject{parent}
{

}

RangeClass::RangeClass(QString rangepath, QObject *parent)
    : QObject{parent},
    m_rangePath(rangepath) // 初始化成员变量 m_rangePath
{
    // 构造函数中尝试加载一次参数
    // 如果加载失败，m_allParams 将被清空或设置为默认的 {"范围参数":{}} 结构
    if (!loadParams()) {
        qWarning() << "RangeClass: Failed to load parameters from" << m_rangePath;
        m_allParams.clear();
        m_allParams["范围参数"] = QVariantMap(); // 确保顶层键存在，以匹配预期的JSON结构
    }
}

// 加载参数到内部成员变量
bool RangeClass::loadParams()
{
    m_allParams = FileOperator::readJsonMap(m_rangePath);

    // 检查最外层的 "范围参数" 键是否存在且为 QVariantMap
    if (m_allParams.contains("范围参数") && m_allParams["范围参数"].type() == QVariant::Map) {
        qDebug() << "RangeClass: Parameters loaded successfully from" << m_rangePath;
        return true;
    } else {
        qWarning() << "RangeClass: Invalid or empty '范围参数' section in" << m_rangePath << ". Initializing with empty map.";
        m_allParams.clear(); // 清空
        m_allParams["范围参数"] = QVariantMap(); // 初始化一次
        return false;
    }
}

bool RangeClass::saveParams()
{
    // 确保 m_allParams 包含了 "范围参数" 这个顶层键，并且其值是 QVariantMap
    if (!m_allParams.contains("范围参数") || m_allParams["范围参数"].type() != QVariant::Map) {
        qWarning() << "RangeClass: Cannot save, '范围参数' section is missing or invalid in internal map.";
        return false;
    }

    if (FileOperator::writeJsonMap(m_rangePath, m_allParams)) {
        qDebug() << "RangeClass: Parameters saved successfully to" << m_rangePath;
        return true;
    } else {
        qWarning() << "RangeClass: Failed to save parameters to" << m_rangePath;
        return false;
    }
}

// 获取某个具体参数的详细信息
QVariantMap RangeClass::getParamDetails(const QString& projectKey, const QString& paramName) const
{
    if (m_allParams.contains("范围参数") && m_allParams["范围参数"].type() == QVariant::Map) {
        QVariantMap rangeMap = m_allParams["范围参数"].toMap();
        if (rangeMap.contains(projectKey) && rangeMap[projectKey].type() == QVariant::Map) {
            QVariantMap projectMap = rangeMap[projectKey].toMap();
            if (projectMap.contains(paramName) && projectMap[paramName].type() == QVariant::Map) {
                return projectMap[paramName].toMap();
            }
        }
    }
    qWarning() << "RangeClass::getParamDetails: Could not find details for param" << paramName << "in project" << projectKey;
    return QVariantMap(); // 如果找不到，返回空Map
}

void RangeClass::updateParamValue(const QString& projectKey, const QString& paramName, const QVariant& newValue)
{
    // 检查并层层深入QVariantMap，直到找到目标参数进行修改
    // 这个过程需要确保每次修改都是在Map的副本上进行，然后重新赋值回父Map
    // 因为QVariantMap::value()返回的是QVariant，toMap()返回的是副本
    // 直接修改副本不会影响原始Map，所以需要层层赋值回去

    if (m_allParams.contains("范围参数") && m_allParams["范围参数"].type() == QVariant::Map) {
        QVariantMap rangeMap = m_allParams["范围参数"].toMap(); // 获取"范围参数"下的Map副本

        if (rangeMap.contains(projectKey) && rangeMap[projectKey].type() == QVariant::Map) {
            QVariantMap projectMap = rangeMap[projectKey].toMap(); // 获取检测项目Map副本

            if (projectMap.contains(paramName) && projectMap[paramName].type() == QVariant::Map) {
                QVariantMap paramDetails = projectMap[paramName].toMap(); // 获取参数详情Map副本

                paramDetails["值"] = newValue; // 更新“值”

                projectMap[paramName] = paramDetails; // 将更新后的参数详情Map放回项目Map副本
                rangeMap[projectKey] = projectMap; // 将更新后的项目Map放回"范围参数"Map副本
                m_allParams["范围参数"] = rangeMap; // 将更新后的"范围参数"Map放回总参数Map

                qDebug() << "RangeClass: Updated parameter:" << projectKey << "->" << paramName << "to" << newValue;
            } else {
                qWarning() << "RangeClass::updateParamValue: Param '" << paramName << "' not found or invalid in project '" << projectKey << "'";
            }
        } else {
            qWarning() << "RangeClass::updateParamValue: Project '" << projectKey << "' not found or invalid in '范围参数'.";
        }
    } else {
        qWarning() << "RangeClass::updateParamValue: '范围参数' section is missing or invalid in m_allParams.";
    }
}

