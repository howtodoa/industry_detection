#ifndef PARAWIDGET_H
#define PARAWIDGET_H

#include <QWidget>
#include "public.h"

class QTabWidget;
class QPushButton;

class ParaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ParaWidget(QWidget *parent = nullptr); // 默认构造函数
    explicit ParaWidget(RangePara* rangeParams, CameralPara* cameralParams, AlgorithmPara* algoParams, QWidget *parent = nullptr);
    ~ParaWidget();

private:
    void setupRangeTab(QTabWidget* tabWidget, RangePara* rangeParams);
    void setupCameralTab(QTabWidget* tabWidget, CameralPara* cameralParams);
    void setupAlgorithmTab(QTabWidget* tabWidget, AlgorithmPara* algoParams);

private slots:
    void saveChanges(); // 保存修改（占位，具体实现待定）
    void closeWindow(); // 关闭窗口

signals:
};

#endif // PARAWIDGET_H
