#include "algoclass_top.h"
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

AlgoClass_Top::AlgoClass_Top(QObject* parent)
    : AlgoClass{ parent }
{
}

AlgoClass_Top::AlgoClass_Top(QString algopath, int al_core, float* Angle, QObject* parent)
    : AlgoClass(algopath, parent),
    al_core(al_core),
    Angle(Angle)
{
    QVariantMap map = FileOperator::readJsonMap(algopath);

    if (map.contains("isExproofExist"))
        LearnPara::inParam3.isExproofExist = ParamDetail(map.value("isExproofExist").toMap()).value.toBool();

    if (map.contains("isPosun"))
        LearnPara::inParam3.isPosun = ParamDetail(map.value("isPosun").toMap()).value.toBool();

    if (map.contains("conf"))
        LearnPara::inParam3.conf = ParamDetail(map.value("conf").toMap()).value.toFloat();

    if (map.contains("nms"))
        LearnPara::inParam3.nms = ParamDetail(map.value("nms").toMap()).value.toFloat();
}

QWidget* AlgoClass_Top::createLeftPanel(QWidget* parent)
{
    QWidget* panel = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(panel);

    // 防爆阀检测
    QHBoxLayout* exproofLayout = new QHBoxLayout;
    QLabel* lblExproof = new QLabel("防爆阀检测:", panel);
    QCheckBox* chkExproof = new QCheckBox(panel);
    chkExproof->setChecked(LearnPara::inParam3.isExproofExist);
    exproofLayout->addWidget(lblExproof);
    exproofLayout->addWidget(chkExproof);
    layout->addLayout(exproofLayout);

    // 破损检测
    QHBoxLayout* posunLayout = new QHBoxLayout;
    QLabel* lblPosun = new QLabel("破损检测:", panel);
    QCheckBox* chkPosun = new QCheckBox(panel);
    chkPosun->setChecked(LearnPara::inParam3.isPosun);
    posunLayout->addWidget(lblPosun);
    posunLayout->addWidget(chkPosun);
    layout->addLayout(posunLayout);

    // 置信度
    QHBoxLayout* confLayout = new QHBoxLayout;
    QLabel* lblConf = new QLabel("置信度:", panel);
    QLineEdit* editConf = new QLineEdit(QString::number(LearnPara::inParam3.conf), panel);
    confLayout->addWidget(lblConf);
    confLayout->addWidget(editConf);
    layout->addLayout(confLayout);

    // 非极大值抑制
    QHBoxLayout* nmsLayout = new QHBoxLayout;
    QLabel* lblNms = new QLabel("NMS:", panel);
    QLineEdit* editNms = new QLineEdit(QString::number(LearnPara::inParam3.nms), panel);
    nmsLayout->addWidget(lblNms);
    nmsLayout->addWidget(editNms);
    layout->addLayout(nmsLayout);

    // 保存按钮
    QPushButton* btnSave = new QPushButton("保存", panel);
    layout->addWidget(btnSave);
    layout->addStretch();

    // 保存按钮更新数据
    connect(btnSave, &QPushButton::clicked, this, [=]() mutable {
        LearnPara::inParam3.isExproofExist = chkExproof->isChecked();
        LearnPara::inParam3.isPosun = chkPosun->isChecked();
        LearnPara::inParam3.conf = editConf->text().toFloat();
        LearnPara::inParam3.nms = editNms->text().toFloat();
        saveParamAsync();
        });

    return panel;
}

void AlgoClass_Top::saveParamAsync()
{
    QVariantMap mapToSave;

    mapToSave["isExproofExist"] = QVariantMap{ {"值", LearnPara::inParam3.isExproofExist} };
    mapToSave["isPosun"] = QVariantMap{ {"值", LearnPara::inParam3.isPosun} };
    mapToSave["conf"] = QVariantMap{ {"值", LearnPara::inParam3.conf} };
    mapToSave["nms"] = QVariantMap{ {"值", LearnPara::inParam3.nms} };

    QString filePath = m_cameralPath;
    QVariantMap dataToSave = mapToSave;

    QtConcurrent::run([filePath, dataToSave]() {
        FileOperator::writeJsonMap(filePath, dataToSave);
        });
}
