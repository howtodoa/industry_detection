#include "algoclass.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>


// 默认构造函数
AlgoClass::AlgoClass(QObject *parent)
    : QObject(parent)
{
    qDebug() << "AlgoClass: 默认构造函数被调用。";
}

// 带参数文件路径的构造函数
AlgoClass::AlgoClass(QString algopath, QObject *parent)
    : QObject(parent), m_cameralPath(algopath) // 初始化 m_cameralPath
{
    qDebug() << "AlgoClass: 带路径的构造函数被调用。路径：" << m_cameralPath;
    loadParams(); // 按照 CameralClass 的模式，在构造时立即加载参数
}

bool AlgoClass::loadParams()
{
    if (m_cameralPath.isEmpty()) {
        qWarning() << "AlgoClass::loadParams: m_cameralPath 为空。无法加载参数。";
        return false;
    }

    QFile file(m_cameralPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "AlgoClass::loadParams: 无法打开文件进行读取：" << m_cameralPath
                   << "错误：" << file.errorString();
        return false;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull()) {
        qWarning() << "AlgoClass::loadParams: 无法从数据创建 JSON 文档。JSON 无效。";
        return false;
    }
    if (!doc.isObject()) {
        qWarning() << "AlgoClass::loadParams: JSON 文档不是一个对象。";
        return false;
    }

    QJsonObject rootObj = doc.object();
    QVariantMap rootMap = rootObj.toVariantMap();

    // 假设你的算法参数在 JSON 文件的根目录下有一个名为 "算法参数" 的顶级键
    const QString topLevelKey = "算法参数";

    if (rootMap.contains(topLevelKey) && rootMap.value(topLevelKey).type() == QVariant::Map) {
        // 直接利用 Parameters 结构体的 QVariantMap 构造函数
        m_parameters = Parameters(rootMap.value(topLevelKey).toMap());
        qDebug() << "AlgoClass::loadParams: 参数从 " << m_cameralPath << " 加载成功。";
        return true;
    } else {
        qWarning() << "AlgoClass::loadParams: JSON 不包含 '" << topLevelKey << "' 键，或者它不是一个对象。";
        return false;
    }
}

bool AlgoClass::saveParams()
{
    if (m_cameralPath.isEmpty()) {
        qWarning() << "AlgoClass::saveParams: m_cameralPath 为空。无法保存参数。";
        return false;
    }

    QVariantMap rootMap;
    // 使用相同的顶级键来封装参数数据
    const QString topLevelKey = "算法参数";
    rootMap[topLevelKey] = m_parameters.toVariantMap(); // 调用 Parameters 结构体的 toVariantMap

    QJsonObject rootObj = QJsonObject::fromVariantMap(rootMap);
    QJsonDocument doc(rootObj);

    QFile file(m_cameralPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qWarning() << "AlgoClass::saveParams: 无法打开文件进行写入：" << m_cameralPath
                   << "错误：" << file.errorString();
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented)); // 以缩进格式写入，方便阅读
    file.close();

    qDebug() << "AlgoClass::saveParams: 参数成功保存到 " << m_cameralPath;
    return true;
}

// getParameters() 是在头文件中内联定义的，因此这里不需要实现

const ParamDetail* AlgoClass::getParamDetail(const QString& projectKey, const QString& paramName) const
{
    // 查找项目
    auto projectIt = m_parameters.detectionProjects.find(projectKey);
    if (projectIt != m_parameters.detectionProjects.end()) {
        // 在项目中查找参数
        auto paramIt = projectIt->params.find(paramName);
        if (paramIt != projectIt->params.end()) {
            return &paramIt.value(); // 返回找到的 ParamDetail 的指针
        }
    }
    return nullptr; // 参数或项目未找到
}

void AlgoClass::updateParamValue(const QString& projectKey, const QString& paramName, const QVariant& newValue)
{
    // 需要使用非 const 引用才能修改 m_parameters 内部数据
    auto projectIt = m_parameters.detectionProjects.find(projectKey);
    if (projectIt != m_parameters.detectionProjects.end()) {
        auto paramIt = projectIt.value().params.find(paramName); // 使用 .value() 获取可修改的 DetectionProject
        if (paramIt != projectIt.value().params.end()) {
            paramIt.value().value = newValue; // 更新值
            qDebug() << "AlgoClass::updateParamValue: 更新了参数值：" << projectKey << "/" << paramName << "为" << newValue;
        } else {
            qWarning() << "AlgoClass::updateParamValue: 参数" << paramName << "在项目" << projectKey << "中未找到。";
        }
    } else {
        qWarning() << "AlgoClass::updateParamValue: 项目" << projectKey << "未找到。";
    }
}

void AlgoClass::updateParamCheck(const QString& projectKey, const QString& paramName, bool checkValue)
{
    // 需要使用非 const 引用才能修改 m_parameters 内部数据
    auto projectIt = m_parameters.detectionProjects.find(projectKey);
    if (projectIt != m_parameters.detectionProjects.end()) {
        auto paramIt = projectIt.value().params.find(paramName); // 使用 .value() 获取可修改的 DetectionProject
        if (paramIt != projectIt.value().params.end()) {
            paramIt.value().check = checkValue; // 更新勾选状态
            qDebug() << "AlgoClass::updateParamCheck: 更新了勾选状态：" << projectKey << "/" << paramName << "为" << checkValue;
        } else {
            qWarning() << "AlgoClass::updateParamCheck: 参数" << paramName << "在项目" << projectKey << "中未找到。";
        }
    } else {
        qWarning() << "AlgoClass::updateParamCheck: 项目" << projectKey << "未找到。";
    }
}
