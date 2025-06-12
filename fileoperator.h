#pragma once

#include <QString>
#include <QJsonObject>
#include <QVariantMap>

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


// 3. 最顶层的范围参数结构体 (Parameters)
// 对应 JSON 中 "范围参数": { "检测项目1": {...}, "检测项目2": {...} } 这一层
struct Parameters
{
    // 存储所有检测项目
    QMap<QString, DetectionProject> detectionProjects;

    // 默认构造函数
    Parameters() = default;

    // 从 QVariantMap 构造的构造函数：方便从 JSON 解析后进行初始化
    // 假设传入的 map 是 {"检测项目1": {...}, "检测项目2": {...}} 这样的结构
    explicit Parameters(const QVariantMap& map)
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


class FileOperator
{
public:
    // 从 JSON 文件读取为 QJsonObject
    static QJsonObject readJsonObject(const QString& filePath);

    // 将 QJsonObject 写入文件
    static bool writeJsonObject(const QString& filePath, const QJsonObject& obj);

    // 从 JSON 文件读取为 QVariantMap
    static QVariantMap readJsonMap(const QString& filePath);

    // 将 QVariantMap 写入文件
    static bool writeJsonMap(const QString& filePath, const QVariantMap& map);
};
