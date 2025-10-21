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
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QMap>
#include <QVariant>
#include <QMessageBox>
#include <QObject>
#include <QtConcurrent/QtConcurrentRun>

AlgoClass_Pin::AlgoClass_Pin(QObject* parent)
	: AlgoClass{ parent }
{}

AlgoClass_Pin::AlgoClass_Pin(QString algopath, int al_core, float* Angle, QObject* parent)
	: AlgoClass(algopath, parent),
	al_core(al_core),
	Angle(Angle)
{
    QVariantMap map = FileOperator::readJsonMap(algopath);
    if (map.contains("pinWidth"))
    {
        ParamDetail detail(map.value("pinWidth").toMap());
        LearnPara::inParam5.pinWidth = detail.value.toFloat();
    }
    if (map.contains("Diameter"))
    {
        ParamDetail detail(map.value("Diameter").toMap());
        LearnPara::inParam5.Diameter = detail.value.toFloat();
    }

    if (map.contains("thresh"))
    {
        ParamDetail detail(map.value("thresh").toMap());
        LearnPara::inParam5.thresh = detail.value.toInt();
    }

    if (map.contains("PinThresh"))
    {
        ParamDetail detail(map.value("PinThresh").toMap());
        LearnPara::inParam5.PinThresh = detail.value.toInt();
    }

    if (map.contains("pinLength"))
    {
        ParamDetail detail(map.value("pinLength").toMap());
        LearnPara::inParam5.pinLength = detail.value.toInt();
    }

    
}

QWidget* AlgoClass_Pin::createLeftPanel(QWidget* parent)
{
    QWidget* panel = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(panel);

    // angleNum
    QHBoxLayout* angleLayout = new QHBoxLayout;
    QLabel* lblAngle = new QLabel("旋转角度:", panel);
    QLineEdit* editAngle = new QLineEdit(QString::number(LearnPara::inParam5.angleNum), panel);
    angleLayout->addWidget(lblAngle);
    angleLayout->addWidget(editAngle);
    layout->addLayout(angleLayout);

    // pinWidth
    QHBoxLayout* widthLayout = new QHBoxLayout;
    QLabel* lblWidth = new QLabel("针脚宽度:", panel);
    QLineEdit* editWidth = new QLineEdit(QString::number(LearnPara::inParam5.pinWidth), panel);
    widthLayout->addWidget(lblWidth);
    widthLayout->addWidget(editWidth);
    layout->addLayout(widthLayout);

    // Diameter
    QHBoxLayout* diaLayout = new QHBoxLayout;
    QLabel* lblDia = new QLabel("定位孔直径:", panel);
    QLineEdit* editDia = new QLineEdit(QString::number(LearnPara::inParam5.Diameter), panel);
    diaLayout->addWidget(lblDia);
    diaLayout->addWidget(editDia);
    layout->addLayout(diaLayout);

    // thresh
    QHBoxLayout* threshLayout = new QHBoxLayout;
    QLabel* lblThresh = new QLabel("胶带二值化参数:", panel);
    QLineEdit* editThresh = new QLineEdit(QString::number(LearnPara::inParam5.thresh), panel);
    threshLayout->addWidget(lblThresh);
    threshLayout->addWidget(editThresh);
    layout->addLayout(threshLayout);

    QHBoxLayout* PinThreshLayout = new QHBoxLayout;
    QLabel* lblPinThresh = new QLabel("胶带上针参数:", panel);
    QLineEdit* editPinThresh = new QLineEdit(QString::number(LearnPara::inParam5.PinThresh), panel);
    PinThreshLayout->addWidget(lblPinThresh);
    PinThreshLayout->addWidget(editPinThresh);
    layout->addLayout(PinThreshLayout);

    QHBoxLayout* pinLengthLayout = new QHBoxLayout;
    QLabel* lblpinLength = new QLabel("针脚长度参数:", panel);
    QLineEdit* editpinLength = new QLineEdit(QString::number(LearnPara::inParam5.pinLength), panel);
    pinLengthLayout->addWidget(lblpinLength);
    pinLengthLayout->addWidget(editpinLength);
    layout->addLayout(pinLengthLayout);

    // 保存按钮
    QPushButton* btnSave = new QPushButton("保存", panel);
    layout->addWidget(btnSave);

    layout->addStretch();

    // 保存按钮点击时更新原参数
    connect(btnSave, &QPushButton::clicked, this, [=]() mutable {
        LearnPara::inParam5.angleNum = editAngle->text().toFloat();
        LearnPara::inParam5.pinWidth = editWidth->text().toFloat();
        LearnPara::inParam5.Diameter = editDia->text().toFloat();
        LearnPara::inParam5.thresh = editThresh->text().toInt();
        LearnPara::inParam5.PinThresh = editPinThresh->text().toInt();
		LearnPara::inParam5.pinLength = editpinLength->text().toInt();
        saveParamAsync();
        });

    return panel;
}

void AlgoClass_Pin::saveParamAsync()
{

    //创建并填充 QVariantMap
    QVariantMap mapToSave;

    mapToSave["pinWidth"] = QVariantMap{ {"值", LearnPara::inParam5.pinWidth} };
    mapToSave["Diameter"] = QVariantMap{ {"值", LearnPara::inParam5.Diameter} };
    mapToSave["thresh"] = QVariantMap{ {"值", LearnPara::inParam5.thresh} };
    mapToSave["PinThresh"] = QVariantMap{ {"值", LearnPara::inParam5.PinThresh} };
    mapToSave["pinLength"] = QVariantMap{ {"值", LearnPara::inParam5.pinLength} };
    // 捕获文件路径和数据
    QString filePath = m_cameralPath;
    QVariantMap dataToSave = mapToSave;

    // 运行异步任务
    QtConcurrent::run([filePath, dataToSave]() {
        if (FileOperator::writeJsonMap(filePath, dataToSave)) {
         //   LOG_DEBUG(GlobalLog::logger, QString("Background thread: Parameters saved successfully to:%1").arg(filePath));
        }
        else {
         // LOG_DEBUG(GlobalLog::logger, QString("Background thread: Failed to save parameters to:%1").arg(filePath));
        }
        });
}
