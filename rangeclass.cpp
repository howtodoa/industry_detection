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
    // 如果加载失败，m_parameters 将保持其默认构造状态（即内部 QMap 为空）
    if (!loadParams()) {
        qWarning() << "RangeClass: Failed to load parameters from" << m_rangePath;
        // m_parameters 默认构造已确保其是空的，无需额外清空或初始化 {"范围参数":{}}
        // 因为 RangeParameters 结构体本身就代表了 "范围参数" 内部的内容
    }
}

// 加载参数到内部结构化成员变量 (m_parameters)
bool RangeClass::loadParams()
{
    // 从文件读取原始的 QVariantMap
    QVariantMap rawDataMap = FileOperator::readJsonMap(m_rangePath);

    // 检查最外层的 "范围参数" 键是否存在且为 QVariantMap
    if (rawDataMap.contains("范围参数") && rawDataMap["范围参数"].type() == QVariant::Map) {
        // 使用 RangeParameters 结构体的构造函数，将 QVariantMap 转换为结构化数据
        m_parameters = RangeParameters(rawDataMap["范围参数"].toMap());
        qDebug() << "RangeClass: Parameters loaded successfully from" << m_rangePath;
        return true;
    } else {
        qWarning() << "RangeClass: Invalid or empty '范围参数' section in" << m_rangePath << ". Initializing with empty parameters.";
        m_parameters = RangeParameters(); // 如果失败或格式不正确，重置为默认空结构体
        return false;
    }
}

// 将内部结构化成员变量 (m_parameters) 保存到文件
bool RangeClass::saveParams()
{
    // 将 RangeParameters 结构体转换回 QVariantMap，用于文件写入
    QVariantMap mapToSave;
    mapToSave["范围参数"] = m_parameters.toVariantMap();

    if (FileOperator::writeJsonMap(m_rangePath, mapToSave)) {
        qDebug() << "RangeClass: Parameters saved successfully to" << m_rangePath;
        return true;
    } else {
        qWarning() << "RangeClass: Failed to save parameters to" << m_rangePath;
        return false;
    }
}

// 获取某个具体参数的详细信息，返回一个指向 ParamDetail 的常量指针。
// 如果找到参数，则返回有效指针；否则返回 nullptr。
const ParamDetail* RangeClass::getParamDetail(const QString& projectKey, const QString& paramName) const
{
    // 查找检测项目
    auto projectIt = m_parameters.detectionProjects.find(projectKey);
    if (projectIt != m_parameters.detectionProjects.end()) {
        const DetectionProject& project = projectIt.value(); // 获取 DetectionProject 引用

        // 查找具体参数
        auto paramIt = project.params.find(paramName);
        if (paramIt != project.params.end()) {
            return &paramIt.value(); // 返回 ParamDetail 的地址
        }
    }
    qWarning() << "RangeClass::getParamDetail: Could not find details for param" << paramName << "in project" << projectKey;
    return nullptr; // 如果找不到，返回空指针
}

// 更新某个具体参数的“值”
void RangeClass::updateParamValue(const QString& projectKey, const QString& paramName, const QVariant& newValue)
{
    // 直接通过结构体访问并修改，更加类型安全和直观
    // 注意：这里需要使用 find() 和 value() 来获取可修改的引用，
    // 或者直接使用 [] 运算符（如果确定键存在且不想处理找不到的情况）
    auto projectIt = m_parameters.detectionProjects.find(projectKey);
    if (projectIt != m_parameters.detectionProjects.end()) {
        DetectionProject& project = projectIt.value(); // 获取 DetectionProject 的引用，以便修改其内部

        auto paramIt = project.params.find(paramName);
        if (paramIt != project.params.end()) {
            ParamDetail& param = paramIt.value(); // 获取 ParamDetail 的引用，以便修改
            param.value = newValue; // 更新值
            qDebug() << "RangeClass: Updated parameter:" << projectKey << "->" << paramName << "to" << newValue;
        } else {
            qWarning() << "RangeClass::updateParamValue: Param '" << paramName << "' not found in project '" << projectKey << "'";
        }
    } else {
        qWarning() << "RangeClass::updateParamValue: Project '" << projectKey << "' not found.";
    }
}

void RangeClass::updateParamCheck(const QString& projectKey, const QString& paramName, bool checkValue)
{
    // 需要获取可修改的引用
    auto projectIt = m_parameters.detectionProjects.find(projectKey);
    if (projectIt != m_parameters.detectionProjects.end()) {
        auto paramIt = projectIt->params.find(paramName);
        if (paramIt != projectIt->params.end()) {
            paramIt.value().check = checkValue; // 直接修改 bool check
            qDebug() << "Updated check for" << projectKey << "/" << paramName << "to" << checkValue;
        } else {
            qWarning() << "RangeClass::updateParamCheck: Parameter" << paramName << "not found in project" << projectKey;
        }
    } else {
        qWarning() << "RangeClass::updateParamCheck: Project" << projectKey << "not found.";
    }
}
