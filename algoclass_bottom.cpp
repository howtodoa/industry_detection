#include "algoclass_bottom.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QtConcurrent/QtConcurrentRun>
#include "typdef.h"
#include "fileoperator.h"

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

    if (map.contains("胶帽标定"))
        LK_BOTTOM_BDparam = ParamDetail(map.value("胶帽标定").toMap()).value.toFloat();

    if (map.contains("铝壳标定"))
        JM_BOTTOM_BDparam = ParamDetail(map.value("铝壳标定").toMap()).value.toFloat();

    // 新增 ROI 参数
    if (map.contains("铝壳ROI阈值"))
        JMROI_Threshold = ParamDetail(map.value("铝壳ROI阈值").toMap()).value.toInt();

    if (map.contains("胶帽ROI阈值"))
        LKROI_Threshold = ParamDetail(map.value("胶帽ROI阈值").toMap()).value.toInt();

    if (map.contains("ROI中心阈值"))
        ROI_center_Threshold = ParamDetail(map.value("ROI中心阈值").toMap()).value.toInt();
}

QWidget* AlgoClass_Bottom::createLeftPanel(QWidget* parent)
{
    QWidget* panel = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(panel);

    auto addRow = [&](const QString& labelText, auto& value) -> QLineEdit* {
        QHBoxLayout* row = new QHBoxLayout;
        QLabel* label = new QLabel(labelText, panel);
        QLineEdit* edit = new QLineEdit(QString::number(value), panel);
        row->addWidget(label);
        row->addWidget(edit);
        layout->addLayout(row);
        return edit;
        };

    // 标定参数
    QLineEdit* editLKBD = addRow("胶帽标定:", LK_BOTTOM_BDparam);
    QLineEdit* editJMBD = addRow("铝壳标定:", JM_BOTTOM_BDparam);

    // 最小值参数
    QLineEdit* editLK = addRow("胶帽最小值:", LK_min);
    QLineEdit* editJM = addRow("铝壳最小值:", JM_min);
    QLineEdit* editWB = addRow("丸棒最小值:", WB_min);

    // ROI 参数
    QLineEdit* editJMROI = addRow("铝壳ROI阈值:", JMROI_Threshold);
    QLineEdit* editLKROI = addRow("胶帽ROI阈值:", LKROI_Threshold);
    QLineEdit* editROIcenter = addRow("ROI中心阈值:", ROI_center_Threshold);

    QPushButton* btnSave = new QPushButton("保存", panel);
    layout->addWidget(btnSave);
    layout->addStretch();

    connect(btnSave, &QPushButton::clicked, this, [=]() mutable {
        LK_BOTTOM_BDparam = editLKBD->text().toFloat();
        JM_BOTTOM_BDparam = editJMBD->text().toFloat();

        LK_min = editLK->text().toFloat();
        JM_min = editJM->text().toFloat();
        WB_min = editWB->text().toFloat();

        JMROI_Threshold = editJMROI->text().toInt();
        LKROI_Threshold = editLKROI->text().toInt();
        ROI_center_Threshold = editROIcenter->text().toInt();

        saveParamAsync();
        });

    return panel;
}

void AlgoClass_Bottom::saveParamAsync()
{
    QVariantMap mapToSave;

    mapToSave["胶帽标定"] = QVariantMap{ {"值", LK_BOTTOM_BDparam} };
    mapToSave["铝壳标定"] = QVariantMap{ {"值", JM_BOTTOM_BDparam} };

    mapToSave["胶帽最小值"] = QVariantMap{ {"值", LK_min} };
    mapToSave["铝壳最小值"] = QVariantMap{ {"值", JM_min} };
    mapToSave["丸棒最小值"] = QVariantMap{ {"值", WB_min} };

    // ROI 参数
    mapToSave["铝壳ROI阈值"] = QVariantMap{ {"值", JMROI_Threshold} };
    mapToSave["胶帽ROI阈值"] = QVariantMap{ {"值", LKROI_Threshold} };
    mapToSave["ROI中心阈值"] = QVariantMap{ {"值", ROI_center_Threshold} };

    QString filePath = m_cameralPath;

    QtConcurrent::run([filePath, mapToSave]() {
        FileOperator::writeJsonMap(filePath, mapToSave);
        });
}
