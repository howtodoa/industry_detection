#pragma once

#include <QString>
#include <QJsonObject>
#include <QVariantMap>

struct ParamDetail
{
    QVariant value;       // "值"：可以是 int, float, QString 等，所以用 QVariant
    QString unit;         // "单位"：如 "mm", "度"
    QString type;         // "类型"：如 "float", "int"
    bool visible;         // "可见"：对应 JSON 中的 "true" / "false" 字符串
    QString range;        // "范围"：如 "1-10", "1-100"
    bool check;           // "检测"：对应 JSON 中的 "true" / "false" 字符串
    QString name;         // 新增字段: 参数的中文映射变量，用于UI显示，对应JSON中的"映射变量"键
    double compensation;  // 新增字段: 补偿值，double类型，用于数值调整

    // 默认构造函数：提供默认值，方便初始化
    ParamDetail()
        : value(),
        unit(""),
        type(""),
        visible(true),
        range(""),
        check(true),
        name(""),
        compensation(0.0) // 初始化新字段，默认为 0.0
    {}

    // 从 QVariantMap 构造的构造函数：方便从 JSON 解析后进行初始化
    // 传入的 map 是 {"值": 10, "单位": "mm", ..., "映射变量": "某个参数的友好中文名称", "补偿值": 0.5} 这样的结构
    explicit ParamDetail(const QVariantMap& map)
    {
        value = map.value("值");
        unit = map.value("单位").toString();
        type = map.value("类型").toString();
        visible = map.value("可见", "true").toString().toLower() == "true"; // 默认可见
        range = map.value("范围").toString();
        check = map.value("检测", "true").toString().toLower() == "true";    // 默认检测
        name = map.value("映射变量").toString(); // 从 map 中读取新字段，键为"映射变量"
        compensation = map.value("补偿值", 0.0).toDouble(); // 从 map 中读取新字段，键为"补偿值"，默认 0.0
    }

    // 将结构体数据转换回 QVariantMap：方便在保存时序列化为 JSON
    QVariantMap toVariantMap() const
    {
        QVariantMap map;
        map["值"] = value;
        map["单位"] = unit;
        map["类型"] = type;
        map["可见"] = visible ? "true" : "false"; // bool 转字符串
        map["范围"] = range;
        map["检测"] = check ? "true" : "false";    // bool 转字符串
        map["映射变量"] = name; // 将新字段写入 map，键为"映射变量"
        map["补偿值"] = compensation; // 将新字段写入 map，键为"补偿值"
        return map;
    }
};

// 2. struct DetectionProject
struct DetectionProject
{
    // 使用 QMap<QString, ParamDetail> 来存储该检测项目下的所有具体参数
    // key 是参数的英文名称（如 "positivePinUpperLimit"），value 是 ParamDetail 结构体
    QMap<QString, ParamDetail> params;

    // 默认构造函数
    DetectionProject() = default;

    // 从 QVariantMap 构造的构造函数：方便从 JSON 解析后进行初始化
    // 假设传入的 map 是 {"positivePinUpperLimit": {...}, "negativePinLowerLimit": {...}} 这样的结构
    explicit DetectionProject(const QVariantMap& map)
    {
        for (auto it = map.begin(); it != map.end(); ++it) {
            // 确保每个值都是一个 QVariantMap，然后用它构造 ParamDetail
            if (it.value().type() == QVariant::Map) {
                params[it.key()] = ParamDetail(it.value().toMap()); // 这里会调用 ParamDetail 新的构造函数
            }
        }
    }

    // 将结构体数据转换回 QVariantMap：方便在保存时序列化为 JSON
    QVariantMap toVariantMap() const
    {
        QVariantMap map;
        for (auto it = params.begin(); it != params.end(); ++it) {
            map[it.key()] = it.value().toVariantMap(); // 这里会调用 ParamDetail 新的 toVariantMap
        }
        return map;
    }
};

// 3. struct Parameters
struct Parameters
{
    // 存储所有检测项目
    // key 是检测项目的英文名称（如 "cameraSettingProject"），value 是 DetectionProject 结构体
    QMap<QString, DetectionProject> detectionProjects;

    // 默认构造函数
    Parameters() = default;

    // 从 QVariantMap 构造的构造函数：方便从 JSON 解析后进行初始化
    // 假设传入的 map 是 {"cameraSettingProject": {...}, "algorithmSettingProject": {...}} 这样的结构
    explicit Parameters(const QVariantMap& map)
    {
        for (auto it = map.begin(); it != map.end(); ++it) {
            if (it.value().type() == QVariant::Map) {
                detectionProjects[it.key()] = DetectionProject(it.value().toMap()); // 调用 DetectionProject 构造函数
            }
        }
    }

    // 将结构体数据转换回 QVariantMap：方便在保存时序列化为 JSON
    QVariantMap toVariantMap() const
    {
        QVariantMap map;
        for (auto it = detectionProjects.begin(); it != detectionProjects.end(); ++it) {
            map[it.key()] = it.value().toVariantMap(); // 调用 DetectionProject 的 toVariantMap
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
