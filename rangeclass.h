#ifndef RANGECLASS_H
#define RANGECLASS_H

#include <QObject>
#include <QVariantMap>
#include <QString>
#include "fileoperator.h"


// 3. 最顶层的范围参数结构体 (RangeParameters)
// 对应 JSON 中 "范围参数": { "检测项目1": {...}, "检测项目2": {...} } 这一层
struct RangeParameters
{
    // 存储所有检测项目
    QMap<QString, DetectionProject> detectionProjects;

    // 默认构造函数
    RangeParameters() = default;

    // 从 QVariantMap 构造的构造函数：方便从 JSON 解析后进行初始化
    // 假设传入的 map 是 {"检测项目1": {...}, "检测项目2": {...}} 这样的结构
    explicit RangeParameters(const QVariantMap& map)
    {
        for (auto it = map.begin(); it != map.end(); ++it) {
            if (it.value().type() == QVariant::Map) {
                detectionProjects[it.key()] = DetectionProject(it.value().toMap());
            }
        }
    }

    // 将结构体数据转换回 QVariantMap：方便在保存时序列化为 JSON
    QVariantMap toVariantMap() const
    {
        QVariantMap map;
        for (auto it = detectionProjects.begin(); it != detectionProjects.end(); ++it) {
            map[it.key()] = it.value().toVariantMap();
        }
        return map;
    }
};



class RangeClass : public QObject
{
    Q_OBJECT
public:
    explicit RangeClass(QObject *parent=nullptr);

    explicit RangeClass(QString rangepath, QObject *parent = nullptr);

    // 加载参数到内部成员变量 (m_parameters)
    // 从 m_rangePath 指定的文件加载，并将 JSON 数据转换为 RangeParameters 结构体
    bool loadParams();
    // 将内部成员变量 (m_parameters) 保存到文件
    // 将 RangeParameters 结构体数据转换回 QVariantMap，然后保存到 m_rangePath 指定的文件
    bool saveParams();


    // 获取所有参数的结构化数据。
    // 返回一个 const 引用，避免不必要的拷贝，并确保外部不能直接修改内部数据。
    const RangeParameters& getRangeParameters() const { return m_parameters; }

    // 获取某个具体参数的详细信息，返回一个指向 ParamDetail 的常量指针。
    // 如果找到参数，则返回有效指针；否则返回 nullptr。
    const ParamDetail* getParamDetail(const QString& projectKey, const QString& paramName) const;


    // 更新某个具体参数的“值”
    // 这个方法将直接修改 m_parameters 结构体内部的对应 ParamDetail 的值
    void updateParamValue(const QString& projectKey, const QString& paramName, const QVariant& newValue);

    void updateParamCheck(const QString& projectKey, const QString& paramName, bool checkValue);

private:
    // 内部保存文件路径，以便加载和保存
    QString m_rangePath;
    // 存储解析后的结构化参数数据
    RangeParameters m_parameters;


};
#endif // RANGECLASS_H
