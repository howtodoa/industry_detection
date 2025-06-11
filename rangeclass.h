#ifndef RANGECLASS_H
#define RANGECLASS_H

#include <QObject>
#include <QVariantMap>
#include <QString>
#include "fileoperator.h"

class RangeClass : public QObject
{
    Q_OBJECT
public:
    explicit RangeClass(QObject *parent = nullptr);
    explicit RangeClass(QString rangepath, QObject *parent = nullptr);

    // 加载参数到内部成员变量
    bool loadParams();
    // 将内部成员变量保存到文件
    bool saveParams();

    // 获取所有参数的副本 (用于UI初始化显示，或者其他类访问)
    // 返回的QVariantMap将是 {"范围参数": {"检测项目1": {...}, ...}} 这种结构
    QVariantMap getAllParams() const;

    // 更新某个具体参数的值
    // projectKey: 例如 "检测项目1" (JSON的第二层键)
    // paramName: 例如 "正极针上限" (JSON的第三层键)
    // newValue: 新的值 (QVariant类型，以便处理不同数据类型)
    void updateParamValue(const QString& projectKey, const QString& paramName, const QVariant& newValue);

    // 获取某个具体参数的详细信息 (用于UI获取“值”、“单位”、“类型”、“可见”等)
    // projectKey: 例如 "检测项目1"
    // paramName: 例如 "正极针上限"
    QVariantMap getParamDetails(const QString& projectKey, const QString& paramName) const;

private:
    // 内部保存文件路径，以便加载和保存
    QString m_rangePath;
    // 保存解析后的所有参数，结构与JSON一致
    // m_allParams 将包含 {"范围参数": {...}} 这种顶层结构
    QVariantMap m_allParams;
};

#endif // RANGECLASS_H
