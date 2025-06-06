#include "FileOperator.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

QJsonObject FileOperator::readJsonObject(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject())
        return {};

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
