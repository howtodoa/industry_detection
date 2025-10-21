#include "FileOperator.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

QJsonObject FileOperator::readJsonObject(const QString& path)
{
    QFile file(path);
    // ��� 1: �ļ��Ƿ�ɹ���
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "����: �޷����ļ���" << path << "ԭ��" << file.errorString();
        return QJsonObject(); // ʧ�ܷ��ؿն���
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    // ��� 2: JSON �����Ƿ�ɹ�
    if (parseError.error != QJsonParseError::NoError) {
        qCritical() << "����: JSON ����ʧ�ܣ�" << path << "ԭ��" << parseError.errorString()
            << "λ��ƫ�ƣ�" << parseError.offset;
        return QJsonObject(); // ʧ�ܷ��ؿն���
    }

    // ��� 3: ���ڵ��Ƿ��Ƕ���
    if (!doc.isObject()) {
        qCritical() << "����: JSON �ļ��ĸ��ڵ㲻��һ�� JSON ���� " << path;
        return QJsonObject(); // ʧ�ܷ��ؿն���
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
