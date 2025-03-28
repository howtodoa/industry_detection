#ifndef PARAWIDGET_H
#define PARAWIDGET_H

#include <QWidget>
#include "public.h"
#include "fileoperator.h"

class QTabWidget;
class QPushButton;

class ParaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ParaWidget(QWidget *parent = nullptr); // 默认构造函数
    explicit ParaWidget(RangePara& rangeParams, CameralPara& cameralParams, AlgorithmPara& algoParams, QWidget *parent = nullptr); // 修改为引用
    ~ParaWidget();

private:
    void setupRangeTab(QTabWidget* tabWidget, RangePara& rangeParams);     // 修改为引用
    void setupCameralTab(QTabWidget* tabWidget, CameralPara& cameralParams); // 修改为引用
    void setupAlgorithmTab(QTabWidget* tabWidget, AlgorithmPara& algoParams); // 修改为引用

private slots:
    void saveCameralChanges(const QString& filePath, const CameralPara& cam);    // 已为引用，无需改动
    void saveRangeChanges(const QString& filePath, const RangePara& range);      // 已为引用，无需改动
    void saveAlgorithmChanges(const QString& filePath, const AlgorithmPara& algo); // 已为引用，无需改动
    void closeWindow(); // 关闭窗口

signals:
};

#endif // PARAWIDGET_H
