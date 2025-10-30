#ifndef DISPLAYINFOWIDGET_H
#define DISPLAYINFOWIDGET_H

#include <QWidget>
#include <QVector>
#include <QMap>
#include "typdef.h" // 假设 Parameters, ProcessedParam, PaintDataItem 在此定义
#include "fileoperator.h"



// 前向声明
class QLabel;
class QGridLayout;

class DisplayInfoWidget : public QWidget
{
    Q_OBJECT

public:
    // 构造函数直接接收原始参数，完成所有初始化工作
    explicit DisplayInfoWidget(Parameters* params, QWidget* parent = nullptr);
    explicit DisplayInfoWidget(const AllUnifyParams& params, QWidget* parent = nullptr);
    ~DisplayInfoWidget();

public slots:
    // 唯一的更新入口，只接收 paintData
    void updateData(const QVector<PaintDataItem>& paintData);

    //  清零计数器的接口
    void resetCounters();

    void onUpdateParameters(Parameters m_parameters);


    void onPaintSend(QVector<PaintDataItem>);

    void onUpdateRealtimeData(const AllUnifyParams& params);

    void updateDataFromUnifyParams(const AllUnifyParams& params);

    //新结构兼容函数
    void onUpdateUnifyParameters(const AllUnifyParams& params);

    void onBuildUIFromUnifyParameters(const AllUnifyParams& params);

private:

    void buildUIFromParameters(Parameters* params);
    void updateLimitLabels(Parameters* params);
    //新结构兼容函数

    void buildUIFromUnifyParams(const AllUnifyParams& params);

    void updateLimitLabelsFromUnifyParams(const AllUnifyParams& params);

    struct TableRow {
        QLabel* nameLabel = nullptr;
        QLabel* lowerLimitLabel = nullptr;
        QLabel* upperLimitLabel = nullptr;
        QLabel* measuredValueLabel = nullptr;
        QLabel* ngCountLabel = nullptr;
    };

    QGridLayout* m_gridLayout;
    QMap<QString, TableRow> m_uiRows;
    QMap<QString, int> m_ngCounters;
};

#endif // DISPLAYINFOWIDGET_H