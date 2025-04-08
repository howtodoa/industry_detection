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
    explicit ParaWidget(QWidget *parent = nullptr);
    explicit ParaWidget(RangePara& rangeParams, CameralPara& cameralParams, AlgorithmPara& algoParams, QWidget *parent = nullptr);
    ~ParaWidget();

private:
    void setupRangeTab(QTabWidget* tabWidget, RangePara& rangeParams);
    void setupCameralTab(QTabWidget* tabWidget, CameralPara& cameralParams);
    void setupAlgorithmTab(QTabWidget* tabWidget, AlgorithmPara& algoParams);

private slots:
    void saveCameralChanges(const QString& filePath, const CameralPara& cam);
    void saveRangeChanges(const QString& filePath, const RangePara& range);
    void saveAlgorithmChanges(const QString& filePath, const AlgorithmPara& algo);
    void closeWindow(); // 关闭窗口

signals:
};

#endif // PARAWIDGET_H
