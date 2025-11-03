#include "algoclass_pin.h"
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
#include <QtConcurrent/QtConcurrentRun>

AlgoClass_Pin::AlgoClass_Pin(QObject* parent)
    : AlgoClass{ parent }
{
}

AlgoClass_Pin::AlgoClass_Pin(QString algopath, int al_core, float* Angle, QObject* parent)
    : AlgoClass(algopath, parent),
    al_core(al_core),
    Angle(Angle)
{
    QVariantMap map = FileOperator::readJsonMap(algopath);

    if (map.contains("pinWidth"))
        LearnPara::inParam5.pinWidth = ParamDetail(map.value("pinWidth").toMap()).value.toFloat();

    if (map.contains("Diameter"))
        LearnPara::inParam5.Diameter = ParamDetail(map.value("Diameter").toMap()).value.toFloat();

    if (map.contains("thresh"))
        LearnPara::inParam5.thresh = ParamDetail(map.value("thresh").toMap()).value.toInt();

    if (map.contains("PinThresh"))
        LearnPara::inParam5.PinThresh = ParamDetail(map.value("PinThresh").toMap()).value.toInt();

    if (map.contains("pinLength"))
        LearnPara::inParam5.pinLength = ParamDetail(map.value("pinLength").toMap()).value.toInt();

    if (map.contains("holeThresh"))
        LearnPara::inParam5.holeThresh = ParamDetail(map.value("holeThresh").toMap()).value.toInt();

    if (map.contains("conf"))
        LearnPara::inParam5.conf = ParamDetail(map.value("conf").toMap()).value.toFloat();

    if (map.contains("nms"))
        LearnPara::inParam5.nms = ParamDetail(map.value("nms").toMap()).value.toFloat();
}

QWidget* AlgoClass_Pin::createLeftPanel(QWidget* parent)
{
    QWidget* panel = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(panel);

    auto addLineEdit = [&](const QString& labelText, float value) {
        QHBoxLayout* hLayout = new QHBoxLayout;
        QLabel* label = new QLabel(labelText, panel);
        QLineEdit* edit = new QLineEdit(QString::number(value), panel);
        hLayout->addWidget(label);
        hLayout->addWidget(edit);
        layout->addLayout(hLayout);
        return edit;
        };

    QLineEdit* editAngle = addLineEdit("旋转角度:", LearnPara::inParam5.angleNum);
    QLineEdit* editWidth = addLineEdit("针脚宽度:", LearnPara::inParam5.pinWidth);
    QLineEdit* editDia = addLineEdit("定位孔直径:", LearnPara::inParam5.Diameter);
    QLineEdit* editThresh = addLineEdit("胶带二值化参数:", LearnPara::inParam5.thresh);
    QLineEdit* editPinThresh = addLineEdit("胶带上针参数:", LearnPara::inParam5.PinThresh);
    QLineEdit* editPinLength = addLineEdit("针脚长度参数:", LearnPara::inParam5.pinLength);
    QLineEdit* editHoleThresh = addLineEdit("定位孔阈值:", LearnPara::inParam5.holeThresh);
    QLineEdit* editConf = addLineEdit("置信度:", LearnPara::inParam5.conf);
    QLineEdit* editNms = addLineEdit("NMS:", LearnPara::inParam5.nms);

    QPushButton* btnSave = new QPushButton("保存", panel);
    layout->addWidget(btnSave);
    layout->addStretch();

    connect(btnSave, &QPushButton::clicked, this, [=]() mutable {
        LearnPara::inParam5.angleNum = editAngle->text().toFloat();
        LearnPara::inParam5.pinWidth = editWidth->text().toFloat();
        LearnPara::inParam5.Diameter = editDia->text().toFloat();
        LearnPara::inParam5.thresh = editThresh->text().toInt();
        LearnPara::inParam5.PinThresh = editPinThresh->text().toInt();
        LearnPara::inParam5.pinLength = editPinLength->text().toInt();
        LearnPara::inParam5.holeThresh = editHoleThresh->text().toInt();
        LearnPara::inParam5.conf = editConf->text().toFloat();
        LearnPara::inParam5.nms = editNms->text().toFloat();
        saveParamAsync();
        });

    return panel;
}

void AlgoClass_Pin::saveParamAsync()
{
    QVariantMap mapToSave;

    mapToSave["pinWidth"] = QVariantMap{ {"值", LearnPara::inParam5.pinWidth} };
    mapToSave["Diameter"] = QVariantMap{ {"值", LearnPara::inParam5.Diameter} };
    mapToSave["thresh"] = QVariantMap{ {"值", LearnPara::inParam5.thresh} };
    mapToSave["PinThresh"] = QVariantMap{ {"值", LearnPara::inParam5.PinThresh} };
    mapToSave["pinLength"] = QVariantMap{ {"值", LearnPara::inParam5.pinLength} };
    mapToSave["holeThresh"] = QVariantMap{ {"值", LearnPara::inParam5.holeThresh} };
    mapToSave["conf"] = QVariantMap{ {"值", LearnPara::inParam5.conf} };
    mapToSave["nms"] = QVariantMap{ {"值", LearnPara::inParam5.nms} };

    QString filePath = m_cameralPath;
    QVariantMap dataToSave = mapToSave;

    QtConcurrent::run([filePath, dataToSave]() {
        FileOperator::writeJsonMap(filePath, dataToSave);
        });
}
