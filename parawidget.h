#ifndef PARAWIDGET_H
#define PARAWIDGET_H

#include <QWidget>
#include <QVariantMap>
#include "fileoperator.h"

class QTabWidget;

class ParaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ParaWidget(const QString& rangePath, const QString& cameralPath, const QString& algoPath, QWidget *parent = nullptr);
    ~ParaWidget();

private:
    void setupRangeTab(QTabWidget* tabWidget, const QVariantMap& rangeParams);
    void setupCameralTab(QTabWidget* tabWidget, const QVariantMap& cameralParams);
    void setupAlgorithmTab(QTabWidget* tabWidget, const QVariantMap& algoParams) ;

private slots:
   // void saveChanges(const QString& filePath); // 保存当前 tab 对应修改
    void closeWindow();

signals:
    void parametersChanged(const QVariantMap& paramMap); //临时保存修改

private:
    QString rangePath;
        QString cameralPath;
        QString algoPath;

    QVariantMap rangeModifiedMap_;
     QVariantMap cameralModifiedMap_ ;
    QVariantMap algoModifiedMap_ ;
};

#endif // PARAWIDGET_H
