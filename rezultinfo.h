#ifndef REZULTINFO_H
#define REZULTINFO_H
#include <QObject>
#include "fileoperator.h"
#include "rangeclass.h"

class RezultInfo : public QObject
{
    Q_OBJECT
public:
    // 构造函数现在需要 RangeParameters 指针作为参数
    explicit RezultInfo(RangeParameters *rangepara, QObject *parent = nullptr);

    // 获取处理后的数据，只包含 'check' 为 true 的参数
    const QMap<QString, QMap<QString, ParamDetail>>& getProcessedData() const;

private:
    // 存储处理后的数据：外层键是检测项目名，内层是参数名到 ParamDetail 的映射
    QMap<QString, QMap<QString, ParamDetail>> m_processedData;

    // 辅助函数，用于处理 RangeParameters 并填充 m_processedData
    void processRangeParameters(RangeParameters *rangePara);
};


#endif // REZULTINFO_H
