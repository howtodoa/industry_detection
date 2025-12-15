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

    // ===== 新增参数（最优先读取）=====
    if (map.contains("颜色过滤参数"))
        XT_ZF_ColorThreshold = map.value("颜色过滤参数").toMap().value("值").toInt();

    if (map.contains("捺印标定值"))
        NY_BDparam = map.value("捺印标定值").toMap().value("值").toFloat();

    // ===== 原有参数 =====
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
}

QWidget* AlgoClass_Ny::createLeftPanel(QWidget* parent)
{
    QWidget* panel = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(panel);

    auto addRowFloat = [&](const QString& labelText, float& value) -> QLineEdit* {
        QHBoxLayout* row = new QHBoxLayout;
        QLabel* label = new QLabel(labelText, panel);
        QLineEdit* edit = new QLineEdit(QString::number(value), panel);
        row->addWidget(label);
        row->addWidget(edit);
        layout->addLayout(row);
        return edit;
        };

    auto addRowInt = [&](const QString& labelText, int& value) -> QLineEdit* {
        QHBoxLayout* row = new QHBoxLayout;
        QLabel* label = new QLabel(labelText, panel);
        QLineEdit* edit = new QLineEdit(QString::number(value), panel);
        row->addWidget(label);
        row->addWidget(edit);
        layout->addLayout(row);
        return edit;
        };

    // ===== 新增参数（放最上面）=====
    QLineEdit* editColorTh = addRowInt("颜色过滤参数:", XT_ZF_ColorThreshold);
    QLineEdit* editBD = addRowFloat("捺印标定值:", NY_BDparam);

    // ===== 原有参数 =====
    QLineEdit* editCQ = addRowFloat("捺印残缺最小面积:", CQ_AreaMin);
    QLineEdit* editGS = addRowFloat("捺印刮伤最小面积:", GS_AreaMin);
    QLineEdit* editHS = addRowFloat("捺印划伤最小面积:", HS_AreaMin);
    QLineEdit* editSeCha = addRowFloat("捺印色差比例:", NY_SeCha_Ratio);
    QLineEdit* editOCR = addRowFloat("字符识别控制参数:", OCR_Control);
    QLineEdit* editQP = addRowFloat("捺印负极气泡最小面积:", QP_AreaMin);
    QLineEdit* editYH = addRowFloat("捺印压痕最小面积:", YH_AreaMin);
    QLineEdit* editZW = addRowFloat("捺印脏污最小面积:", ZW_AreaMin);

    QPushButton* btnSave = new QPushButton("保存", panel);
    layout->addWidget(btnSave);
    layout->addStretch();

    connect(btnSave, &QPushButton::clicked, this, [=]() mutable {
        XT_ZF_ColorThreshold = editColorTh->text().toInt();
        NY_BDparam = editBD->text().toFloat();

        CQ_AreaMin = editCQ->text().toFloat();
        GS_AreaMin = editGS->text().toFloat();
        HS_AreaMin = editHS->text().toFloat();
        NY_SeCha_Ratio = editSeCha->text().toFloat();
        OCR_Control = editOCR->text().toFloat();
        QP_AreaMin = editQP->text().toFloat();
        YH_AreaMin = editYH->text().toFloat();
        ZW_AreaMin = editZW->text().toFloat();

        saveParamAsync();
        });

    return panel;
}

void AlgoClass_Ny::saveParamAsync()
{
    QVariantMap mapToSave;

    // ===== 新增参数 =====
    mapToSave["颜色过滤参数"] = QVariantMap{ {"值", XT_ZF_ColorThreshold} };
    mapToSave["捺印标定值"] = QVariantMap{ {"值", NY_BDparam} };

    // ===== 原有参数 =====
    mapToSave["捺印残缺最小面积"] = QVariantMap{ {"值", CQ_AreaMin} };
    mapToSave["捺印刮伤最小面积"] = QVariantMap{ {"值", GS_AreaMin} };
    mapToSave["捺印划伤最小面积"] = QVariantMap{ {"值", HS_AreaMin} };
    mapToSave["捺印色差比例"] = QVariantMap{ {"值", NY_SeCha_Ratio} };
    mapToSave["字符识别控制参数"] = QVariantMap{ {"值", OCR_Control} };
    mapToSave["捺印负极气泡最小面积"] = QVariantMap{ {"值", QP_AreaMin} };
    mapToSave["捺印压痕最小面积"] = QVariantMap{ {"值", YH_AreaMin} };
    mapToSave["捺印脏污最小面积"] = QVariantMap{ {"值", ZW_AreaMin} };

    QString filePath = m_cameralPath;

    QtConcurrent::run([filePath, mapToSave]() {
        FileOperator::writeJsonMap(filePath, mapToSave);
        });
}
