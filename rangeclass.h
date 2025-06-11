#ifndef RANGECLASS_H
#define RANGECLASS_H

#include <QObject>
#include <QVariantMap>
#include <QString>
#include "fileoperator.h"

struct ParamDetail
{
    QVariant value;    // "值"：可以是 int, float, QString 等，所以用 QVariant
    QString unit;      // "单位"：如 "mm", "度"
    QString type;      // "类型"：如 "float", "int"
    bool visible;      // "可见"：对应 JSON 中的 "true" / "false" 字符串
    QString range;     // "范围"：如 "1-10", "1-100"
    bool check;        // "检测"：对应 JSON 中的 "true" / "false" 字符串

    // 默认构造函数：提供默认值，方便初始化
    ParamDetail() : value(), unit(""), type(""), visible(true), range(""), check(true) {}

    // 从 QVariantMap 构造的构造函数：方便从 JSON 解析后进行初始化
    // 传入的 map 是 {"值": 10, "单位": "mm", ...} 这样的结构
    explicit ParamDetail(const QVariantMap& map)
    {
        value = map.value("值"); // QVariant 可以直接赋值，无需 toInt() 或 toFloat()
        unit = map.value("单位").toString();
        type = map.value("类型").toString();
        // 将 JSON 中的 "true"/"false" 字符串转换为 bool 类型
        visible = map.value("可见", "true").toString().toLower() == "true";
        range = map.value("范围").toString();
        check = map.value("检测", "true").toString().toLower() == "true";
    }

    // 将结构体数据转换回 QVariantMap：方便在保存时序列化为 JSON
    QVariantMap toVariantMap() const
    {
        QVariantMap map;
        map["值"] = value;
        map["单位"] = unit;
        map["类型"] = type;
        // 将 bool 类型转换回 "true" / "false" 字符串，以匹配原始 JSON 格式
        map["可见"] = visible ? "true" : "false";
        map["范围"] = range;
        map["检测"] = check ? "true" : "false";
        return map;
    }
};



struct DetectionProject
{
    // 使用 QMap<QString, ParamDetail> 来存储该检测项目下的所有具体参数
    // key 是参数名（如 "正极针上限"），value 是 ParamDetail 结构体
    QMap<QString, ParamDetail> params;

    // 默认构造函数
    DetectionProject() = default; // 默认构造函数，不需要特殊初始化

    // 从 QVariantMap 构造的构造函数：方便从 JSON 解析后进行初始化
    // 假设传入的 map 是 {"正极针上限": {...}, "正极针中限": {...}} 这样的结构
    explicit DetectionProject(const QVariantMap& map)
    {
        for (auto it = map.begin(); it != map.end(); ++it) {
            // 确保每个值都是一个 QVariantMap，然后用它构造 ParamDetail
            if (it.value().type() == QVariant::Map) {
                params[it.key()] = ParamDetail(it.value().toMap());
            }
        }
    }

    // 将结构体数据转换回 QVariantMap：方便在保存时序列化为 JSON
    QVariantMap toVariantMap() const
    {
        QVariantMap map;
        for (auto it = params.begin(); it != params.end(); ++it) {
            map[it.key()] = it.value().toVariantMap(); // 调用 ParamDetail 的 toVariantMap
        }
        return map;
    }
};


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
