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
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QMap>
#include <QVariant>
#include <QMessageBox>
#include <QObject>
#include <QtConcurrent/QtConcurrentRun>

AlgoClass_Side::AlgoClass_Side(QObject* parent)
    : AlgoClass{ parent }
{}

AlgoClass_Side::AlgoClass_Side(QString algopath, int al_core, float* Angle, QObject* parent)
    : AlgoClass(algopath, parent),
    al_core(al_core),
    Angle(Angle)
{
    QVariantMap map = FileOperator::readJsonMap(algopath);

    if (map.contains("charAngleNum"))
    {
        ParamDetail detail(map.value("charAngleNum").toMap());
        LearnPara::inParam4.charAngleNum = detail.value.toFloat();
    }
    if (map.contains("buttonThresh"))
    {
        ParamDetail detail(map.value("buttonThresh").toMap());
        LearnPara::inParam4.buttonThresh = detail.value.toInt();
    }
    if (map.contains("topThresh"))
    {
        ParamDetail detail(map.value("topThresh").toMap());
        LearnPara::inParam4.topThresh = detail.value.toInt();
    }
    if (map.contains("isColor"))
    {
        ParamDetail detail(map.value("isColor").toMap());
        LearnPara::inParam4.isColor = detail.value.toBool();
    }
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

    // 保存按钮点击时更新原参数
    connect(btnSave, &QPushButton::clicked, this, [=]() mutable {
        LearnPara::inParam4.charAngleNum = editCharAngle->text().toFloat();
        LearnPara::inParam4.buttonThresh = editButtonThresh->text().toInt();
        LearnPara::inParam4.topThresh = editTopThresh->text().toInt();
        LearnPara::inParam4.isColor = chkColor->isChecked();
        saveParamAsync();
        });

    return panel;
}

void AlgoClass_Side::saveParamAsync()
{
    // 创建并填充 QVariantMap
    QVariantMap mapToSave;

    mapToSave["charAngleNum"] = QVariantMap{ {"值", LearnPara::inParam4.charAngleNum} };
    mapToSave["buttonThresh"] = QVariantMap{ {"值", LearnPara::inParam4.buttonThresh} };
    mapToSave["topThresh"] = QVariantMap{ {"值", LearnPara::inParam4.topThresh} };
    mapToSave["isColor"] = QVariantMap{ {"值", LearnPara::inParam4.isColor} };

    // 捕获文件路径和数据
    QString filePath = m_cameralPath;
    QVariantMap dataToSave = mapToSave;

    // 运行异步任务
    QtConcurrent::run([filePath, dataToSave]() {
        FileOperator::writeJsonMap(filePath, dataToSave);
        });
}
