#include "algoclass_ny.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QtConcurrent/QtConcurrentRun>
#include "typdef.h"
#include "FileOperator.h"

AlgoClass_Ny::AlgoClass_Ny(QObject* parent)
    : AlgoClass(parent)
{
}

AlgoClass_Ny::AlgoClass_Ny(QString algopath, QObject* parent)
    : AlgoClass(algopath, parent)
{
    QVariantMap map = FileOperator::readJsonMap(algopath);

    if (map.contains("CQ_AreaMin"))
        CQ_AreaMin = ParamDetail(map.value("CQ_AreaMin").toMap()).value.toFloat();

    if (map.contains("GS_AreaMin"))
        GS_AreaMin = ParamDetail(map.value("GS_AreaMin").toMap()).value.toFloat();

    if (map.contains("HS_AreaMin"))
        HS_AreaMin = ParamDetail(map.value("HS_AreaMin").toMap()).value.toFloat();

    if (map.contains("QP_AreaMin"))
        QP_AreaMin = ParamDetail(map.value("QP_AreaMin").toMap()).value.toFloat();

    if (map.contains("YH_AreaMin"))
        YH_AreaMin = ParamDetail(map.value("YH_AreaMin").toMap()).value.toFloat();

    if (map.contains("ZW_AreaMin"))
        ZW_AreaMin = ParamDetail(map.value("ZW_AreaMin").toMap()).value.toFloat();

    if (map.contains("SC_Ratio"))
        SC_Ratio = ParamDetail(map.value("SC_Ratio").toMap()).value.toFloat();

    if (map.contains("OCR_Control"))
        OCR_Control = ParamDetail(map.value("OCR_Control").toMap()).value.toFloat();
}

QWidget* AlgoClass_Ny::createLeftPanel(QWidget* parent)
{
    QWidget* panel = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(panel);

    auto addParamRow = [&](const QString& labelText, float& param) {
        QHBoxLayout* row = new QHBoxLayout;
        QLabel* lbl = new QLabel(labelText, panel);
        QLineEdit* edit = new QLineEdit(QString::number(param), panel);
        row->addWidget(lbl);
        row->addWidget(edit);
        layout->addLayout(row);
        return edit;
        };

    QLineEdit* editCQ = addParamRow("CQ_AreaMin:", CQ_AreaMin);
    QLineEdit* editGS = addParamRow("GS_AreaMin:", GS_AreaMin);
    QLineEdit* editHS = addParamRow("HS_AreaMin:", HS_AreaMin);
    QLineEdit* editQP = addParamRow("QP_AreaMin:", QP_AreaMin);
    QLineEdit* editYH = addParamRow("YH_AreaMin:", YH_AreaMin);
    QLineEdit* editZW = addParamRow("ZW_AreaMin:", ZW_AreaMin);
    QLineEdit* editSC = addParamRow("SC_Ratio:", SC_Ratio);
    QLineEdit* editOCR = addParamRow("OCR_Control:", OCR_Control);

    QPushButton* btnSave = new QPushButton("保存", panel);
    layout->addWidget(btnSave);
    layout->addStretch();

    connect(btnSave, &QPushButton::clicked, this, [=]() mutable {
        CQ_AreaMin = editCQ->text().toFloat();
        GS_AreaMin = editGS->text().toFloat();
        HS_AreaMin = editHS->text().toFloat();
        QP_AreaMin = editQP->text().toFloat();
        YH_AreaMin = editYH->text().toFloat();
        ZW_AreaMin = editZW->text().toFloat();
        SC_Ratio = editSC->text().toFloat();
        OCR_Control = editOCR->text().toFloat();
        saveParamAsync();
        });

    return panel;
}

void AlgoClass_Ny::saveParamAsync()
{
    QVariantMap mapToSave;

    mapToSave["CQ_AreaMin"] = QVariantMap{ {"值", CQ_AreaMin} };
    mapToSave["GS_AreaMin"] = QVariantMap{ {"值", GS_AreaMin} };
    mapToSave["HS_AreaMin"] = QVariantMap{ {"值", HS_AreaMin} };
    mapToSave["QP_AreaMin"] = QVariantMap{ {"值", QP_AreaMin} };
    mapToSave["YH_AreaMin"] = QVariantMap{ {"值", YH_AreaMin} };
    mapToSave["ZW_AreaMin"] = QVariantMap{ {"值", ZW_AreaMin} };
    mapToSave["SC_Ratio"] = QVariantMap{ {"值", SC_Ratio} };
    mapToSave["OCR_Control"] = QVariantMap{ {"值", OCR_Control} };

    QString filePath = m_cameralPath;
    QVariantMap dataToSave = mapToSave;

    QtConcurrent::run([filePath, dataToSave]() {
        FileOperator::writeJsonMap(filePath, dataToSave);
        });
}
