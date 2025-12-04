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

    // ------- 读取中文 Key -------
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

    // ------- 使用中文 KEY 写回 JSON -------
    mapToSave["胶帽最小值"] = QVariantMap{ {"值", LK_min} };
    mapToSave["铝壳最小值"] = QVariantMap{ {"值", JM_min} };
    mapToSave["丸棒最小值"] = QVariantMap{ {"值", WB_min} };

    QString filePath = m_cameralPath;

    QtConcurrent::run([filePath, mapToSave]() {
        FileOperator::writeJsonMap(filePath, mapToSave);
        });
}
