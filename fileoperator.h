#pragma once

#include <QString>
#include <QJsonObject>
#include <QVariantMap>

class FileOperator
{
public:
    // 从 JSON 文件读取为 QJsonObject
    static QJsonObject readJsonObject(const QString& filePath);

    // 将 QJsonObject 写入文件
    static bool writeJsonObject(const QString& filePath, const QJsonObject& obj);

    // 从 JSON 文件读取为 QVariantMap（适合 UI 操作）
    static QVariantMap readJsonMap(const QString& filePath);

    // 将 QVariantMap 写入文件
    static bool writeJsonMap(const QString& filePath, const QVariantMap& map);
};
