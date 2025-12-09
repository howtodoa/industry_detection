#include "algoclass_xs.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QtConcurrent/QtConcurrentRun>
#include "typdef.h"
#include "fileoperator.h"

AlgoClass_Xs::AlgoClass_Xs(QObject* parent)
    : AlgoClass(parent)
{
}

AlgoClass_Xs::AlgoClass_Xs(QString algopath, QObject* parent)
    : AlgoClass(algopath, parent)
{
    QVariantMap map = FileOperator::readJsonMap(algopath);

    if (map.contains("塑胶最小宽度"))
        SJ_WidthMin = map.value("塑胶最小宽度").toMap().value("值").toFloat();

    if (map.contains("塑胶最大宽度"))
        SJ_WidthMax = map.value("塑胶最大宽度").toMap().value("值").toFloat();

    if (map.contains("胶帽高度"))
        JM_Height = map.value("胶帽高度").toMap().value("值").toFloat();

    if (map.contains("胶帽亮度上阈值"))
        JM_Thresholdup = map.value("胶帽亮度上阈值").toMap().value("值").toInt();

    if (map.contains("胶帽亮度下阈值"))
        JM_Thresholddown = map.value("胶帽亮度下阈值").toMap().value("值").toInt();

    if (map.contains("凹陷最大面积"))
        AX_max = map.value("凹陷最大面积").toMap().value("值").toFloat();

    if (map.contains("凹陷面积"))
        CMAX_max = map.value("凹陷面积").toMap().value("值").toFloat();

    if (map.contains("划伤最大面积"))
        HS_max = map.value("划伤最大面积").toMap().value("值").toFloat();

    if (map.contains("脏污最大面积"))
        ZW_max = map.value("脏污最大面积").toMap().value("值").toFloat();

    // ⭐ 新增：线扫标定值
    if (map.contains("线扫标定值"))
        XS_BDparam = map.value("线扫标定值").toMap().value("值").toFloat();
}

QWidget* AlgoClass_Xs::createLeftPanel(QWidget* parent)
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

    QLineEdit* editSJMin = addRow("塑胶最小宽度:", SJ_WidthMin);
    QLineEdit* editSJMax = addRow("塑胶最大宽度:", SJ_WidthMax);
    QLineEdit* editJMHeight = addRow("胶帽高度:", JM_Height);
    QLineEdit* editJMUp = addRow("胶帽亮度上阈值:", JM_Thresholdup);
    QLineEdit* editJMDown = addRow("胶帽亮度下阈值:", JM_Thresholddown);
    QLineEdit* editAXMax = addRow("凹陷最大面积:", AX_max);
    QLineEdit* editCMAX = addRow("凹陷面积:", CMAX_max);
    QLineEdit* editHSMax = addRow("划伤最大面积:", HS_max);
    QLineEdit* editZWMax = addRow("脏污最大面积:", ZW_max);

    // ⭐ 新增 UI 输入框：线扫标定值
    QLineEdit* editBDValue = addRow("线扫标定值:", XS_BDparam);

    QPushButton* btnSave = new QPushButton("保存", panel);
    layout->addWidget(btnSave);
    layout->addStretch();

    connect(btnSave, &QPushButton::clicked, this, [=]() mutable {
        SJ_WidthMin = editSJMin->text().toFloat();
        SJ_WidthMax = editSJMax->text().toFloat();
        JM_Height = editJMHeight->text().toFloat();
        JM_Thresholdup = editJMUp->text().toInt();
        JM_Thresholddown = editJMDown->text().toInt();
        AX_max = editAXMax->text().toFloat();
        CMAX_max = editCMAX->text().toFloat();
        HS_max = editHSMax->text().toFloat();
        ZW_max = editZWMax->text().toFloat();

        // ⭐ 保存新参数
        XS_BDparam = editBDValue->text().toFloat();

        saveParamAsync();
        });

    return panel;
}

void AlgoClass_Xs::saveParamAsync()
{
    QVariantMap mapToSave;

    mapToSave["塑胶最小宽度"] = QVariantMap{ {"值", SJ_WidthMin} };
    mapToSave["塑胶最大宽度"] = QVariantMap{ {"值", SJ_WidthMax} };
    mapToSave["胶帽高度"] = QVariantMap{ {"值", JM_Height} };
    mapToSave["胶帽亮度上阈值"] = QVariantMap{ {"值", JM_Thresholdup} };
    mapToSave["胶帽亮度下阈值"] = QVariantMap{ {"值", JM_Thresholddown} };
    mapToSave["凹陷最大面积"] = QVariantMap{ {"值", AX_max} };
    mapToSave["凹陷面积"] = QVariantMap{ {"值", CMAX_max} };
    mapToSave["划伤最大面积"] = QVariantMap{ {"值", HS_max} };
    mapToSave["脏污最大面积"] = QVariantMap{ {"值", ZW_max} };

    // ⭐ 新增 JSON 字段：线扫标定值
    mapToSave["线扫标定值"] = QVariantMap{ {"值", XS_BDparam} };

    QString filePath = m_cameralPath;
    QVariantMap dataToSave = mapToSave;

    QtConcurrent::run([filePath, dataToSave]() {
        FileOperator::writeJsonMap(filePath, dataToSave);
        });
}
