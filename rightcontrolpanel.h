#ifndef RIGHTCONTROLPANEL_H
#define RIGHTCONTROLPANEL_H

#include <QWidget>
#include <QVector>
#include "typdef.h" 
#include <QObject>

class Cameral;
class QLabel;
class QGroupBox;
class QLineEdit;

class RightControlPanel : public QWidget
{
    Q_OBJECT

public:
    // 构造函数接收 cams (数据源) 和 caminfo (元数据) 用于“绑定”
    explicit RightControlPanel(const QVector<Cameral*>& cams, const QVector<Camerinfo>& caminfo, QWidget* parent = nullptr);
    ~RightControlPanel();

public slots:
    void updateStatistics();
    void setMachineId(const QString& id);
    void startDBChart();
signals:
    void photoAllClicked();
    void startAllClicked();
    void AllSheild(bool check);
    void stopAllClicked();
    void clearAllClicked();
    void parameterButtonClicked(int buttonIndex); // 0:顶面, 1:侧面, 2:引脚

private:
    void setupUi(const QVector<Camerinfo>& caminfo);
    QGroupBox* createMachineParamsArea();
    QGroupBox* createParameterButtonsArea(const QVector<Camerinfo>& caminfo);
    QGroupBox* createStatisticsArea(const QVector<Camerinfo>& caminfo);
    QWidget* createBottomButtonsArea();
    QGroupBox* createAllShieldButtonArea();

    // 成员变量
    const QVector<Cameral*>& m_cams; 
    QLabel* m_machineIdLabel;       
    QString Color="#007BFF";
    // 用于管理统计数据的标签
    QVector<QLabel*> m_yieldRateLabels;
    QVector<QLabel*> m_ngCountLabels;
    QVector<QLabel*> m_totalCountLabels;
    QLineEdit* m_machineIdEdit = nullptr;
};

#endif // RIGHTCONTROLPANEL_H