#include "FileOperator.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

QJsonObject FileOperator::readJsonObject(const QString& path)
{
    QFile file(path);
    // 检查 1: 文件是否成功打开
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "错误: 无法打开文件：" << path << "原因：" << file.errorString();
        return QJsonObject(); // 失败返回空对象
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    // 检查 2: JSON 解析是否成功
    if (parseError.error != QJsonParseError::NoError) {
        qCritical() << "错误: JSON 解析失败：" << path << "原因：" << parseError.errorString()
            << "位于偏移：" << parseError.offset;
        return QJsonObject(); // 失败返回空对象
    }

    // 检查 3: 根节点是否是对象
    if (!doc.isObject()) {
        qCritical() << "错误: JSON 文件的根节点不是一个 JSON 对象： " << path;
        return QJsonObject(); // 失败返回空对象
    }

    return doc.object();
}

bool FileOperator::writeJsonObject(const QString& filePath, const QJsonObject& obj)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QJsonDocument doc(obj);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

QVariantMap FileOperator::readJsonMap(const QString& filePath)
{
    return readJsonObject(filePath).toVariantMap();
}

bool FileOperator::writeJsonMap(const QString& filePath, const QVariantMap& map)
{
    QJsonObject obj = QJsonObject::fromVariantMap(map);
    return writeJsonObject(filePath, obj);
}
