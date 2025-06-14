#ifndef PARAWIDGET_H
#define PARAWIDGET_H

#include <QWidget>
#include <QVariantMap>
#include "rangeclass.h"
#include "fileoperator.h"
#include "cameralclass.h"
#include "algoclass.h"
#include <QCheckBox>
#include <QLineEdit>
#include<QMap>


class QTabWidget;

class ParaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ParaWidget( RangeClass *RC, CameralClass * CC, AlgoClass *AC,QWidget *parent = nullptr);
    ~ParaWidget();

private:
    void setupRangeTab(QTabWidget* tabWidget);
    void setupCameralTab(QTabWidget* tabWidget);
    void setupAlgorithmTab(QTabWidget* tabWidget) ;
    void setupDebugTab(QTabWidget* tabWidget) ;

private slots:

    void closeWindow();

signals:
    void parametersChanged(const QVariantMap& paramMap); //临时保存修改

private:

     RangeClass* m_rangeSettings;
     CameralClass * m_cameralSettings;
     AlgoClass *m_algoSettings;
     QMap<QString, QMap<QString, QLineEdit*>> m_paramValueEdits;
     QMap<QString, QMap<QString, QCheckBox*>> m_paramCheckboxes;

     QMap<QString, QMap<QString, QLineEdit*>> m_cameralValueEdits;  // 存储相机参数的 QLineEdit
     QMap<QString, QMap<QString, QCheckBox*>> m_cameralCheckboxes;  // 存储相机参数的 QCheckBox

     QMap<QString, QMap<QString, QLineEdit*>> m_algoValueEdits;
     QMap<QString, QMap<QString, QCheckBox*>> m_algoCheckboxes;

 private slots:
     void onCalibClicked();
     void onROIClicked();
     void onRecipeClicked();
};

#endif // PARAWIDGET_H
