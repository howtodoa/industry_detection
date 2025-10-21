#ifndef RANGECLASS_H
#define RANGECLASS_H

#include <QObject>
#include <QVariantMap>
#include <QString>
#include <QFuture>         
#include <QFutureWatcher> 
#include "fileoperator.h"
#include "typdef.h"



class RangeClass : public QObject
{
    Q_OBJECT
public:
    explicit RangeClass(QObject *parent = nullptr);
    explicit RangeClass(QString rangepath, QObject *parent = nullptr);

    // 加载参数到内部成员变量 (m_parameters)
    // 从 m_rangePath 指定的文件加载，并将 JSON 数据转换为 Parameters 结构体
    bool loadParams();
    // 将内部成员变量 (m_parameters) 保存到文件
    // 将 Parameters 结构体数据转换回 QVariantMap，然后保存到 m_rangePath 指定的文件
    void saveParamsAsync();

    // 获取所有参数的结构化数据。
    // 返回一个 const 引用，避免不必要的拷贝，并确保外部不能直接修改内部数据。
    const Parameters& getRangeParameters() const { return m_parameters; }
    const Parameters& getParameters() const { return m_parameters; }

    // 获取某个具体参数的详细信息，返回一个指向 ParamDetail 的常量指针。
    // 如果找到参数，则返回有效指针；否则返回 nullptr。
    const ParamDetail* getParamDetail(const QString& projectKey, const QString& paramName) const;

    // 更新某个具体参数的“值”
    // 这个方法将直接修改 m_parameters 结构体内部的对应 ParamDetail 的值
    void updateParamValue(const QString& projectKey, const QString& paramName, const QVariant& newValue);
    void updateParamCompensation(const QString& projectName, const QString& paramName, double compensationValue); // <-- 在这里添加这个声明
    void updateParamCheck(const QString& projectKey, const QString& paramName, bool checkValue);

    static UnifyParam parseSingleParam(const QJsonObject& paramObject);
    static AllUnifyParams loadUnifiedParameters(const QString& jsonFilePath);
public:
    // 内部保存文件路径，以便加载和保存
    QString m_rangePath;
    // 存储解析后的结构化参数数据
    Parameters m_parameters; // 

};


#endif // RANGECLASS_H
