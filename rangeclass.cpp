#include "fileoperator.h"
#include "rangeclass.h" // 包含更新后的 RangeClass 头文件，其中 RangeParameters 已更名为 Parameters

#include <QObject>
#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtConcurrent/QtConcurrentRun>

RangeClass::RangeClass(QObject *parent)
    : QObject{parent}
{

}

RangeClass::RangeClass(QString rangepath, QObject *parent)
    : QObject{parent},
    m_rangePath(rangepath) // 初始化成员变量 m_rangePath
{
    // 在构造函数内部调用 loadParams() 来立即加载数据
    if (!m_rangePath.isEmpty()) {
        if (!loadParams()) {
            qDebug() << "RangeClass: Failed to load parameters from" << m_rangePath; // 修正了多余的 's'
            // 可以在这里设置默认参数或采取其他错误处理措施
        }
    } else {
        qDebug() << "RangeClass: rangepath is empty, parameters not loaded."; // 修正了多余的 's'
    }
}

// 将内部结构化成员变量 (m_parameters) 保存到文件
bool RangeClass::loadParams()
{
    // 从文件读取原始的 QVariantMap
    QVariantMap rawDataMap = FileOperator::readJsonMap(m_rangePath);

    // 检查最外层的 "范围参数" 键是否存在且为 QVariantMap
    if (rawDataMap.contains("范围参数") && rawDataMap["范围参数"].type() == QVariant::Map) {
        // 使用 Parameters 结构体的构造函数，将 QVariantMap 转换为结构化数据
        m_parameters = Parameters(rawDataMap["范围参数"].toMap());
        qDebug() << "RangeClass: Parameters loaded successfully from" << m_rangePath;
        return true;
    }
    else {
        qWarning() << "RangeClass: Invalid or empty '范围参数' section in" << m_rangePath << ". Initializing with empty parameters.";
        m_parameters = Parameters(); 
        return false;
    }
}


void RangeClass::saveParamsAsync() // Changed from bool saveParams()
{
    qDebug() << "RangeClass: Starting async save to" << m_rangePath;

    QVariantMap mapToSave;
    mapToSave["范围参数"] = m_parameters.toVariantMap();
    QString filePath = m_rangePath;

    QFuture<bool> future = QtConcurrent::run([filePath, dataToSave = mapToSave]() {
        qDebug() << "Background thread: Executing RangeClass parameter save...";
        if (FileOperator::writeJsonMap(filePath, dataToSave)) {
            qDebug() << "Background thread: Parameters saved successfully to" << filePath;
            return true;
        }
        else {
            qWarning() << "Background thread: Failed to save parameters to" << filePath;
            return false;
        }
        });

    QFutureWatcher<bool>* watcher = new QFutureWatcher<bool>(this);

    watcher->setFuture(future);

    qDebug() << "RangeClass: Async save initiated, main thread continues.";
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
void RangeClass::updateParamCompensation(const QString& projectName, const QString& paramName, double compensationValue)
{
    if (m_parameters.detectionProjects.contains(projectName)) {
        DetectionProject& project = m_parameters.detectionProjects[projectName]; // Get a modifiable reference
        if (project.params.contains(paramName)) {
            ParamDetail& detail = project.params[paramName]; // Get a modifiable reference
            detail.compensation = compensationValue;
            qDebug() << "Updated compensation for" << projectName << "/" << paramName << "to" << compensationValue;
        }
        else {
            qWarning() << "Param" << paramName << "not found in project" << projectName;
        }
    }
    else {
        qWarning() << "Project" << projectName << "not found.";
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
