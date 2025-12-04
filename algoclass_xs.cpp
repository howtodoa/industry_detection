#include "algoclass_xs.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QtConcurrent/QtConcurrentRun>
#include "typdef.h"
#include "FileOperator.h"

AlgoClass_Xs::AlgoClass_Xs(QObject* parent)
    : AlgoClass(parent)
{
}

AlgoClass_Xs::AlgoClass_Xs(QString algopath, QObject* parent)
    : AlgoClass(algopath, parent)
{
    QVariantMap map = FileOperator::readJsonMap(algopath);

    if (map.contains("SJ_WidthMin")) SJ_WidthMin = map.value("SJ_WidthMin").toMap().value("值").toFloat();
    if (map.contains("SJ_WidthMax")) SJ_WidthMax = map.value("SJ_WidthMax").toMap().value("值").toFloat();
    if (map.contains("JM_Height")) JM_Height = map.value("JM_Height").toMap().value("值").toFloat();
    if (map.contains("JM_Thresholdup")) JM_Thresholdup = map.value("JM_Thresholdup").toMap().value("值").toInt();
    if (map.contains("JM_Thresholddown")) JM_Thresholddown = map.value("JM_Thresholddown").toMap().value("值").toInt();

    if (map.contains("AX_max")) AX_max = map.value("AX_max").toMap().value("值").toFloat();
    if (map.contains("CMAX_max")) CMAX_max = map.value("CMAX_max").toMap().value("值").toFloat();
    if (map.contains("HS_max")) HS_max = map.value("HS_max").toMap().value("值").toFloat();
    if (map.contains("ZW_max")) ZW_max = map.value("ZW_max").toMap().value("值").toFloat();
}

QWidget* AlgoClass_Xs::createLeftPanel(QWidget* parent)
{
    QWidget* panel = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(panel);

    auto addFloatEdit = [&](const QString& name, float& value) {
        QHBoxLayout* hLayout = new QHBoxLayout;
        QLabel* lbl = new QLabel(name, panel);
        QLineEdit* edit = new QLineEdit(QString::number(value), panel);
        hLayout->addWidget(lbl);
        hLayout->addWidget(edit);
        layout->addLayout(hLayout);
        return edit;
        };

    auto addIntEdit = [&](const QString& name, int& value) {
        QHBoxLayout* hLayout = new QHBoxLayout;
        QLabel* lbl = new QLabel(name, panel);
        QLineEdit* edit = new QLineEdit(QString::number(value), panel);
        hLayout->addWidget(lbl);
        hLayout->addWidget(edit);
        layout->addLayout(hLayout);
        return edit;
        };

    QLineEdit* editSJ_WidthMin = addFloatEdit("SJ_WidthMin:", SJ_WidthMin);
    QLineEdit* editSJ_WidthMax = addFloatEdit("SJ_WidthMax:", SJ_WidthMax);
    QLineEdit* editJM_Height = addFloatEdit("JM_Height:", JM_Height);
    QLineEdit* editJM_Thresholdup = addIntEdit("JM_Thresholdup:", JM_Thresholdup);
    QLineEdit* editJM_Thresholddown = addIntEdit("JM_Thresholddown:", JM_Thresholddown);

    QLineEdit* editAX_max = addFloatEdit("AX_max:", AX_max);
    QLineEdit* editCMAX_max = addFloatEdit("CMAX_max:", CMAX_max);
    QLineEdit* editHS_max = addFloatEdit("HS_max:", HS_max);
    QLineEdit* editZW_max = addFloatEdit("ZW_max:", ZW_max);

    QPushButton* btnSave = new QPushButton("保存", panel);
    layout->addWidget(btnSave);
    layout->addStretch();

    connect(btnSave, &QPushButton::clicked, this, [=]() mutable {
        SJ_WidthMin = editSJ_WidthMin->text().toFloat();
        SJ_WidthMax = editSJ_WidthMax->text().toFloat();
        JM_Height = editJM_Height->text().toFloat();
        JM_Thresholdup = editJM_Thresholdup->text().toInt();
        JM_Thresholddown = editJM_Thresholddown->text().toInt();
        AX_max = editAX_max->text().toFloat();
        CMAX_max = editCMAX_max->text().toFloat();
        HS_max = editHS_max->text().toFloat();
        ZW_max = editZW_max->text().toFloat();
        saveParamAsync();
        });

    return panel;
}

void AlgoClass_Xs::saveParamAsync()
{
    QVariantMap mapToSave;
    mapToSave["SJ_WidthMin"] = QVariantMap{ {"值", SJ_WidthMin} };
    mapToSave["SJ_WidthMax"] = QVariantMap{ {"值", SJ_WidthMax} };
    mapToSave["JM_Height"] = QVariantMap{ {"值", JM_Height} };
    mapToSave["JM_Thresholdup"] = QVariantMap{ {"值", JM_Thresholdup} };
    mapToSave["JM_Thresholddown"] = QVariantMap{ {"值", JM_Thresholddown} };

    mapToSave["AX_max"] = QVariantMap{ {"值", AX_max} };
    mapToSave["CMAX_max"] = QVariantMap{ {"值", CMAX_max} };
    mapToSave["HS_max"] = QVariantMap{ {"值", HS_max} };
    mapToSave["ZW_max"] = QVariantMap{ {"值", ZW_max} };

    QString filePath = m_cameralPath;
    QVariantMap dataToSave = mapToSave;

    QtConcurrent::run([filePath, dataToSave]() {
        FileOperator::writeJsonMap(filePath, dataToSave);
        });
}
