#include "algoclass_side.h"
#include <QObject>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include "typdef.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDialog>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QtConcurrent/QtConcurrentRun>

AlgoClass_Side::AlgoClass_Side(QObject* parent)
    : AlgoClass{ parent }
{
}

AlgoClass_Side::AlgoClass_Side(QString algopath, int al_core, float* Angle, QObject* parent)
    : AlgoClass(algopath, parent),
    al_core(al_core),
    Angle(Angle)
{
    QVariantMap map = FileOperator::readJsonMap(algopath);

    if (map.contains("charAngleNum"))
        LearnPara::inParam4.charAngleNum = ParamDetail(map.value("charAngleNum").toMap()).value.toFloat();

    if (map.contains("buttonThresh"))
        LearnPara::inParam4.buttonThresh = ParamDetail(map.value("buttonThresh").toMap()).value.toInt();

    if (map.contains("topThresh"))
        LearnPara::inParam4.topThresh = ParamDetail(map.value("topThresh").toMap()).value.toInt();

    if (map.contains("isColor"))
        LearnPara::inParam4.isColor = ParamDetail(map.value("isColor").toMap()).value.toBool();

    if (map.contains("conf"))
        LearnPara::inParam4.conf = ParamDetail(map.value("conf").toMap()).value.toFloat();

    if (map.contains("nms"))
        LearnPara::inParam4.nms = ParamDetail(map.value("nms").toMap()).value.toFloat();
}

QWidget* AlgoClass_Side::createLeftPanel(QWidget* parent)
{
    QWidget* panel = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(panel);

    // 字符旋转角度
    QHBoxLayout* charAngleLayout = new QHBoxLayout;
    QLabel* lblCharAngle = new QLabel("字符角度:", panel);
    QLineEdit* editCharAngle = new QLineEdit(QString::number(LearnPara::inParam4.charAngleNum), panel);
    charAngleLayout->addWidget(lblCharAngle);
    charAngleLayout->addWidget(editCharAngle);
    layout->addLayout(charAngleLayout);

    // 编带下层二值化
    QHBoxLayout* buttonThreshLayout = new QHBoxLayout;
    QLabel* lblButtonThresh = new QLabel("下层二值化:", panel);
    QLineEdit* editButtonThresh = new QLineEdit(QString::number(LearnPara::inParam4.buttonThresh), panel);
    buttonThreshLayout->addWidget(lblButtonThresh);
    buttonThreshLayout->addWidget(editButtonThresh);
    layout->addLayout(buttonThreshLayout);

    // 编带上层二值化
    QHBoxLayout* topThreshLayout = new QHBoxLayout;
    QLabel* lblTopThresh = new QLabel("上层二值化:", panel);
    QLineEdit* editTopThresh = new QLineEdit(QString::number(LearnPara::inParam4.topThresh), panel);
    topThreshLayout->addWidget(lblTopThresh);
    topThreshLayout->addWidget(editTopThresh);
    layout->addLayout(topThreshLayout);

    // 置信度
    QHBoxLayout* confLayout = new QHBoxLayout;
    QLabel* lblConf = new QLabel("置信度:", panel);
    QLineEdit* editConf = new QLineEdit(QString::number(LearnPara::inParam4.conf), panel);
    confLayout->addWidget(lblConf);
    confLayout->addWidget(editConf);
    layout->addLayout(confLayout);

    // 非极大值抑制
    QHBoxLayout* nmsLayout = new QHBoxLayout;
    QLabel* lblNms = new QLabel("NMS:", panel);
    QLineEdit* editNms = new QLineEdit(QString::number(LearnPara::inParam4.nms), panel);
    nmsLayout->addWidget(lblNms);
    nmsLayout->addWidget(editNms);
    layout->addLayout(nmsLayout);

    // 套管颜色判定
    QHBoxLayout* colorLayout = new QHBoxLayout;
    QLabel* lblColor = new QLabel("颜色判定:", panel);
    QCheckBox* chkColor = new QCheckBox(panel);
    chkColor->setChecked(LearnPara::inParam4.isColor);
    colorLayout->addWidget(lblColor);
    colorLayout->addWidget(chkColor);
    layout->addLayout(colorLayout);

    // 保存按钮
    QPushButton* btnSave = new QPushButton("保存", panel);
    layout->addWidget(btnSave);
    layout->addStretch();

    // 保存按钮更新数据
    connect(btnSave, &QPushButton::clicked, this, [=]() mutable {
        LearnPara::inParam4.charAngleNum = editCharAngle->text().toFloat();
        LearnPara::inParam4.buttonThresh = editButtonThresh->text().toInt();
        LearnPara::inParam4.topThresh = editTopThresh->text().toInt();
        LearnPara::inParam4.isColor = chkColor->isChecked();
        LearnPara::inParam4.conf = editConf->text().toFloat();
        LearnPara::inParam4.nms = editNms->text().toFloat();
        saveParamAsync();
        });

    return panel;
}

void AlgoClass_Side::saveParamAsync()
{
    QVariantMap mapToSave;
    
    mapToSave["charAngleNum"] = QVariantMap{ {"值", LearnPara::inParam4.charAngleNum} };
    mapToSave["buttonThresh"] = QVariantMap{ {"值", LearnPara::inParam4.buttonThresh} };
    mapToSave["topThresh"] = QVariantMap{ {"值", LearnPara::inParam4.topThresh} };
    mapToSave["isColor"] = QVariantMap{ {"值", LearnPara::inParam4.isColor} };
    mapToSave["conf"] = QVariantMap{ {"值", LearnPara::inParam4.conf} };
    mapToSave["nms"] = QVariantMap{ {"值", LearnPara::inParam4.nms} };

    QString filePath = m_cameralPath;
    QVariantMap dataToSave = mapToSave;

    QtConcurrent::run([filePath, dataToSave]() {
        FileOperator::writeJsonMap(filePath, dataToSave);
        });
}
