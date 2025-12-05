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
#include <QCloseEvent>
#include <QLayout>
#include<QMap>
#include<QTabWidget>
#include "cameral.h"
#include "typdef.h"
#include "imageviewerwindow.h"


class QTabWidget;

class ParaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ParaWidget( RangeClass *RC, CameralClass * CC, AlgoClass *AC,Cameral *cam,DebugInfo *DI,QWidget *parent = nullptr);
    ~ParaWidget();
    void closeEvent(QCloseEvent* event) override;
private:
    void setupRangeTab(QTabWidget* tabWidget);
    void setupRangeTab_EX(QTabWidget* tabWidget);
    void setupCameralTab(QTabWidget* tabWidget);
    void setupAlgorithmTab(QTabWidget* tabWidget) ;
    void setupDebugTab(QTabWidget* tabWidget) ;
    void setupScaleTab(QTabWidget* tabWidget);
    void validateInputType(QLineEdit* lineEdit, const QString& type);
    void saveScaleArrayAsync(const QString& filePath, const QVector<SimpleParam>& scaleArray);
    void saveScaleParamsToFile(const AllUnifyParams& paramsToSave);
    void setupScaleTabEX(QTabWidget* tabWidget);

private slots:

    void closeWindow();
    void onScaleSaveClicked();
    void saveRangePara();
signals:
    void parametersChanged(const QVariantMap& paramMap); //临时保存修改
    void SaveDebugInfo(DebugInfo* DI);
    void ImageLoaded(std::shared_ptr<cv::Mat> image);
    void Learn();
    void Check();
private:

    // 1. 通用错误处理模块
    void handleSettingsInitializationError(QTabWidget* tabWidget, const QString& tabName, const QString& errorMessage);
   //2.处理参数为空的情况
    void displayNoParametersMessage(QVBoxLayout* mainLayout, const QString& settingType);
    // T 是 RangeClass, AlgoClass, CameralClass 中的任意一种
    template<typename T>
    QString validateParametersRange(T* settingsManager, const QString& settingType) const;
    void saveParamsToJsonFile(const AllUnifyParams& paramsToSave);

    // 3. 通用 QLineEdit 验证器设置 (只依赖 ParamDetail)
    // 注意：这里的 ParamDetail 不包含 name，name 需要在外部传入或从 QMap 的 key 获取
    void setupLineEditValidator(QLineEdit* valueEdit, const ParamDetail& paramDetail, const QString& paramNameForLog); // 增加一个参数用于日志或错误信息

     RangeClass* m_rangeSettings;
     CameralClass * m_cameralSettings;
     AlgoClass *m_algoSettings;
      
     QMap<QString, QMap<QString, QLineEdit*>> m_paramValueEdits;
     QMap<QString, QMap<QString, QCheckBox*>> m_paramCheckboxes;
     QMap<QString, QMap<QString, QLineEdit*>> m_paramCompensationEdits; // 
     QMap<QString, QLineEdit*> m_lineEditMap;
     QMap<QString, QComboBox*> m_paramBoolModeComboBoxes;
     QMap<QString, QLineEdit*> m_rangeLineEditMap;
     QMap<QString, QMap<QString, QCheckBox*>> m_rangeCheckboxes;
     QMap<QString, QCheckBox*> m_scaleCheckboxes;   
     QMap<QString, QLineEdit*> m_scaleLineEditMap;
     QMap<QString, QCheckBox*> m_scaleEnableCheckboxes;
     QMap<QString, QLineEdit*> m_selfLearnLineEditMap;

     DebugInfo *DI;
     Cameral *m_cam=nullptr;
     QTabWidget* tabWidget = nullptr;
public:
    ImageViewerWindow* imageViewer;
    ImageViewerWindow* imageViewer_algo;
 private slots:
     void onCalibClicked();
     void onROIClicked();
     void onRecipeClicked();
     void onChooseImageClicked();
     void onTransMat(cv::Mat);
public slots:
     void onSaveClicked();
};

#endif // PARAWIDGET_H
