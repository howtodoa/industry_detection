#include "algoclass_bottom.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QtConcurrent/QtConcurrentRun>
#include "typdef.h"

AlgoClass_Bottom::AlgoClass_Bottom(QObject* parent)
    : AlgoClass(parent)
{
}

AlgoClass_Bottom::AlgoClass_Bottom(QString algopath, QObject* parent)
    : AlgoClass(algopath, parent)
{
    QVariantMap map = FileOperator::readJsonMap(algopath);

    // ------- 标定参数读取 -------
    if (map.contains("胶帽标定"))
        LK_BOTTOM_BDparam = ParamDetail(map.value("胶帽标定").toMap()).value.toFloat();

    if (map.contains("铝壳标定"))
        JM_BOTTOM_BDparam = ParamDetail(map.value("铝壳标定").toMap()).value.toFloat();

    // ------- 原有参数 -------
    if (map.contains("胶帽最小值"))
        LK_min = ParamDetail(map.value("胶帽最小值").toMap()).value.toFloat();

    if (map.contains("铝壳最小值"))
        JM_min = ParamDetail(map.value("铝壳最小值").toMap()).value.toFloat();

    if (map.contains("丸棒最小值"))
        WB_min = ParamDetail(map.value("丸棒最小值").toMap()).value.toFloat();
}

QWidget* AlgoClass_Bottom::createLeftPanel(QWidget* parent)
{
    QWidget* panel = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(panel);

    // *************** 标定参数放最上面 ***************
    // --- 胶帽标定 ---
    QHBoxLayout* lkBDLayout = new QHBoxLayout;
    QLabel* lblLKBD = new QLabel("胶帽标定:", panel);
    QLineEdit* editLKBD = new QLineEdit(QString::number(LK_BOTTOM_BDparam), panel);
    lkBDLayout->addWidget(lblLKBD);
    lkBDLayout->addWidget(editLKBD);
    layout->addLayout(lkBDLayout);

    // --- 铝壳标定 ---
    QHBoxLayout* jmBDLayout = new QHBoxLayout;
    QLabel* lblJMBD = new QLabel("铝壳标定:", panel);
    QLineEdit* editJMBD = new QLineEdit(QString::number(JM_BOTTOM_BDparam), panel);
    jmBDLayout->addWidget(lblJMBD);
    jmBDLayout->addWidget(editJMBD);
    layout->addLayout(jmBDLayout);

    // *************** 原有参数 ***************
    // --- 胶帽最小值 ---
    QHBoxLayout* lkLayout = new QHBoxLayout;
    QLabel* lblLK = new QLabel("胶帽最小值:", panel);
    QLineEdit* editLK = new QLineEdit(QString::number(LK_min), panel);
    lkLayout->addWidget(lblLK);
    lkLayout->addWidget(editLK);
    layout->addLayout(lkLayout);

    // --- 铝壳最小值 ---
    QHBoxLayout* jmLayout = new QHBoxLayout;
    QLabel* lblJM = new QLabel("铝壳最小值:", panel);
    QLineEdit* editJM = new QLineEdit(QString::number(JM_min), panel);
    jmLayout->addWidget(lblJM);
    jmLayout->addWidget(editJM);
    layout->addLayout(jmLayout);

    // --- 丸棒最小值 ---
    QHBoxLayout* wbLayout = new QHBoxLayout;
    QLabel* lblWB = new QLabel("丸棒最小值:", panel);
    QLineEdit* editWB = new QLineEdit(QString::number(WB_min), panel);
    wbLayout->addWidget(lblWB);
    wbLayout->addWidget(editWB);
    layout->addLayout(wbLayout);

    // 保存按钮
    QPushButton* btnSave = new QPushButton("保存", panel);
    layout->addWidget(btnSave);
    layout->addStretch();

    connect(btnSave, &QPushButton::clicked, this, [=]() {
        LK_BOTTOM_BDparam = editLKBD->text().toFloat();
        JM_BOTTOM_BDparam = editJMBD->text().toFloat();
        LK_min = editLK->text().toFloat();
        JM_min = editJM->text().toFloat();
        WB_min = editWB->text().toFloat();
        saveParamAsync();
        });

    return panel;
}

void AlgoClass_Bottom::saveParamAsync()
{
    QVariantMap mapToSave;

    // ------- 标定参数 -------
    mapToSave["胶帽标定"] = QVariantMap{ {"值", LK_BOTTOM_BDparam} };
    mapToSave["铝壳标定"] = QVariantMap{ {"值", JM_BOTTOM_BDparam} };

    // ------- 原有参数 -------
    mapToSave["胶帽最小值"] = QVariantMap{ {"值", LK_min} };
    mapToSave["铝壳最小值"] = QVariantMap{ {"值", JM_min} };
    mapToSave["丸棒最小值"] = QVariantMap{ {"值", WB_min} };

    QString filePath = m_cameralPath;

    QtConcurrent::run([filePath, mapToSave]() {
        FileOperator::writeJsonMap(filePath, mapToSave);
        });
}
