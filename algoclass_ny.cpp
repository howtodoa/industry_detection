#include "algoclass_ny.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QtConcurrent/QtConcurrentRun>
#include "typdef.h"
#include "fileoperator.h"

AlgoClass_Ny::AlgoClass_Ny(QObject* parent)
    : AlgoClass(parent)
{
}

AlgoClass_Ny::AlgoClass_Ny(QString algopath, QObject* parent)
    : AlgoClass(algopath, parent)
{
    QVariantMap map = FileOperator::readJsonMap(algopath);

    if (map.contains("捺印残缺最小面积"))
        CQ_AreaMin = map.value("捺印残缺最小面积").toMap().value("值").toFloat();

    if (map.contains("捺印刮伤最小面积"))
        GS_AreaMin = map.value("捺印刮伤最小面积").toMap().value("值").toFloat();

    if (map.contains("捺印划伤最小面积"))
        HS_AreaMin = map.value("捺印划伤最小面积").toMap().value("值").toFloat();

    if (map.contains("捺印色差比例"))
        NY_SeCha_Ratio = map.value("捺印色差比例").toMap().value("值").toFloat();

    if (map.contains("字符识别控制参数"))
        OCR_Control = map.value("字符识别控制参数").toMap().value("值").toFloat();

    if (map.contains("捺印负极气泡最小面积"))
        QP_AreaMin = map.value("捺印负极气泡最小面积").toMap().value("值").toFloat();

    if (map.contains("捺印压痕最小面积"))
        YH_AreaMin = map.value("捺印压痕最小面积").toMap().value("值").toFloat();

    if (map.contains("捺印脏污最小面积"))
        ZW_AreaMin = map.value("捺印脏污最小面积").toMap().value("值").toFloat();

    // 新增参数
    if (map.contains("型替字符相似度阈值"))
        XT_Similarity_Threshold = map.value("型替字符相似度阈值").toMap().value("值").toFloat();

    if (map.contains("型替字符HSV_H"))
        NY_H = map.value("型替字符HSV_H").toMap().value("值").toInt();

    if (map.contains("型替字符HSV_S"))
        NY_S = map.value("型替字符HSV_S").toMap().value("值").toInt();
}

QWidget* AlgoClass_Ny::createLeftPanel(QWidget* parent)
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

    QLineEdit* editCQ = addRow("捺印残缺最小面积:", CQ_AreaMin);
    QLineEdit* editGS = addRow("捺印刮伤最小面积:", GS_AreaMin);
    QLineEdit* editHS = addRow("捺印划伤最小面积:", HS_AreaMin);
    QLineEdit* editSeCha = addRow("捺印色差比例:", NY_SeCha_Ratio);
    QLineEdit* editOCR = addRow("字符识别控制参数:", OCR_Control);
    QLineEdit* editQP = addRow("捺印负极气泡最小面积:", QP_AreaMin);
    QLineEdit* editYH = addRow("捺印压痕最小面积:", YH_AreaMin);
    QLineEdit* editZW = addRow("捺印脏污最小面积:", ZW_AreaMin);

    // 新增三个参数
    QLineEdit* editSimThreshold = addRow("型替字符相似度阈值:", XT_Similarity_Threshold);
    QLineEdit* editH = addRow("型替字符HSV_H:", NY_H);
    QLineEdit* editS = addRow("型替字符HSV_S:", NY_S);

    QLineEdit* editBD = addRow("捺印标定:", NY_BDparam);
    QLineEdit* editColorThreshold = addRow("颜色过滤参数:", XT_ZF_ColorThreshold);

    QPushButton* btnSave = new QPushButton("保存", panel);
    layout->addWidget(btnSave);
    layout->addStretch();

    connect(btnSave, &QPushButton::clicked, this, [=]() mutable {
        CQ_AreaMin = editCQ->text().toFloat();
        GS_AreaMin = editGS->text().toFloat();
        HS_AreaMin = editHS->text().toFloat();
        NY_SeCha_Ratio = editSeCha->text().toFloat();
        OCR_Control = editOCR->text().toFloat();
        QP_AreaMin = editQP->text().toFloat();
        YH_AreaMin = editYH->text().toFloat();
        ZW_AreaMin = editZW->text().toFloat();

        XT_Similarity_Threshold = editSimThreshold->text().toFloat();
        NY_H = editH->text().toInt();
        NY_S = editS->text().toInt();

        NY_BDparam = editBD->text().toFloat();
        XT_ZF_ColorThreshold = editColorThreshold->text().toInt();

        saveParamAsync();
        });

    return panel;
}

void AlgoClass_Ny::saveParamAsync()
{
    QVariantMap mapToSave;

    mapToSave["捺印残缺最小面积"] = QVariantMap{ {"值", CQ_AreaMin} };
    mapToSave["捺印刮伤最小面积"] = QVariantMap{ {"值", GS_AreaMin} };
    mapToSave["捺印划伤最小面积"] = QVariantMap{ {"值", HS_AreaMin} };
    mapToSave["捺印色差比例"] = QVariantMap{ {"值", NY_SeCha_Ratio} };
    mapToSave["字符识别控制参数"] = QVariantMap{ {"值", OCR_Control} };
    mapToSave["捺印负极气泡最小面积"] = QVariantMap{ {"值", QP_AreaMin} };
    mapToSave["捺印压痕最小面积"] = QVariantMap{ {"值", YH_AreaMin} };
    mapToSave["捺印脏污最小面积"] = QVariantMap{ {"值", ZW_AreaMin} };

    // 新增三个参数
    mapToSave["型替字符相似度阈值"] = QVariantMap{ {"值", XT_Similarity_Threshold} };
    mapToSave["型替字符HSV_H"] = QVariantMap{ {"值", NY_H} };
    mapToSave["型替字符HSV_S"] = QVariantMap{ {"值", NY_S} };

    mapToSave["捺印标定"] = QVariantMap{ {"值", NY_BDparam} };
    mapToSave["颜色过滤参数"] = QVariantMap{ {"值", XT_ZF_ColorThreshold} };

    QString filePath = m_cameralPath;
    QVariantMap dataToSave = mapToSave;

    QtConcurrent::run([filePath, dataToSave]() {
        FileOperator::writeJsonMap(filePath, dataToSave);
        });
}
