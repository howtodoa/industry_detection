#include "DebugTool.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include "typdef.h"
#include "fileoperator.h"
#include <MZ_VC3000H.h>
#include <MZ_VC5000.h>
#include <thread>

DebugTool::DebugTool(QWidget* parent)
    : QWidget(parent)
{
    this->setWindowTitle("开发者工具");
    this->resize(500, 450);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // === 字体大小 ===
    QHBoxLayout* fontSizeLayout = new QHBoxLayout;
    fontSizeLayout->setSpacing(5);
    QPushButton* fontSizeButton = new QPushButton("字体大小", this);
    fontSizeButton->setFixedSize(100, 30);
    fontSizeSlider = new QSlider(Qt::Horizontal, this);
    fontSizeSlider->setRange(10, 30);
    fontSizeSlider->setValue(GlobalPara::FontSize);
    fontSizeValueLabel = new QLabel(QString::number(fontSizeSlider->value()), this);
    fontSizeValueLabel->setFixedWidth(30);
    fontSizeValueLabel->setAlignment(Qt::AlignCenter);
    fontSizeLayout->addWidget(fontSizeButton);
    fontSizeLayout->addWidget(fontSizeSlider);
    fontSizeLayout->addWidget(fontSizeValueLabel);
    fontSizeLayout->addStretch();
    mainLayout->addLayout(fontSizeLayout);

    // === LIGHT1~LIGHT4 ===
    for (int i = 1; i <= 4; ++i) {
        QHBoxLayout* lightLayout = new QHBoxLayout;
        lightLayout->setSpacing(5);
        QPushButton* lightButton = new QPushButton(QString("LIGHT%1").arg(i), this);
        lightButton->setFixedSize(100, 30);
        QSlider* slider = new QSlider(Qt::Horizontal, this);
        slider->setRange(1, 100);
        int value = 30;
        switch (i) {
        case 1: value = GlobalPara::Light1; light1Slider = slider; break;
        case 2: value = GlobalPara::Light2; light2Slider = slider; break;
        case 3: value = GlobalPara::Light3; light3Slider = slider; break;
        case 4: value = GlobalPara::Light4; light4Slider = slider; break;
        }
        slider->setValue(value);
        QLabel* valueLabel = new QLabel(QString::number(value), this);
        valueLabel->setFixedWidth(30);
        valueLabel->setAlignment(Qt::AlignCenter);
        lightLayout->addWidget(lightButton);
        lightLayout->addWidget(slider);
        lightLayout->addWidget(valueLabel);
        lightLayout->addStretch();
        mainLayout->addLayout(lightLayout);

        // 实时显示数值
        connect(slider, &QSlider::valueChanged, this, [valueLabel](int v) {
            valueLabel->setText(QString::number(v));
            });
    }

    // === 运行点位 RunPoint ===
    QHBoxLayout* runPointLayout = new QHBoxLayout;
    runPointLayout->setSpacing(5);
    QLabel* runPointLabel = new QLabel("运行点位", this);
    runPointLabel->setFixedSize(100, 30);
    runPointCombo = new QComboBox(this);
    for (int i = 1; i <= 30; ++i) runPointCombo->addItem(QString::number(i));
    runPointCombo->setCurrentIndex(GlobalPara::RunPoint - 1);
    runPointLayout->addWidget(runPointLabel);
    runPointLayout->addWidget(runPointCombo);
    runPointLayout->addStretch();
    mainLayout->addLayout(runPointLayout);

    // === 输入点位 InputPoint ===
    QHBoxLayout* inputPointLayout = new QHBoxLayout;
    inputPointLayout->setSpacing(5);
    QLabel* inputPointLabel = new QLabel("输入点位", this);
    inputPointLabel->setFixedSize(100, 30);
    inputPointCombo = new QComboBox(this);
    for (int i = 0; i <= 8; ++i) inputPointCombo->addItem(QString::number(i));
    inputPointCombo->setCurrentIndex(GlobalPara::InputPoint - 1);
    inputPointLayout->addWidget(inputPointLabel);
    inputPointLayout->addWidget(inputPointCombo);
    inputPointLayout->addStretch();
    mainLayout->addLayout(inputPointLayout);

    // === 断开相机按钮 ===
    QHBoxLayout* disconnectLayout = new QHBoxLayout;
    disconnectLayout->setSpacing(5);
    disconnectCameraButton = new QPushButton("断开相机", this);
    disconnectCameraButton->setFixedSize(160, 30);
    disconnectLayout->addWidget(disconnectCameraButton);
    disconnectLayout->addStretch();
    mainLayout->addLayout(disconnectLayout);

    // === 自学习次数 ===
    QHBoxLayout* learnLayout = new QHBoxLayout;
    learnCountButton = new QPushButton("自学习次数", this);
    learnCountButton->setFixedWidth(100);
    learnCountEdit = new QLineEdit(this);
    learnCountEdit->setFixedWidth(50);
    learnCountEdit->setText(QString::number(GlobalPara::LearnCount));
    learnLayout->addWidget(learnCountButton);
    learnLayout->addWidget(learnCountEdit);
    learnLayout->addStretch();
    mainLayout->addLayout(learnLayout);

    // === 超时时间 ===
    QHBoxLayout* timeoutLayout = new QHBoxLayout;
    timeoutButton = new QPushButton("超时时间", this);
    timeoutButton->setFixedWidth(100);
    timeoutEdit = new QLineEdit(this);
    timeoutEdit->setFixedWidth(80);
    timeoutEdit->setText(QString::number(GlobalPara::TimeOut));
    timeoutLayout->addWidget(timeoutButton);
    timeoutLayout->addWidget(timeoutEdit);
    timeoutLayout->addStretch();
    mainLayout->addLayout(timeoutLayout);

    // === 点位输出 ===
    QHBoxLayout* outputLayout = new QHBoxLayout;
    outputLayout->setSpacing(5);

    QLabel* outputLabel = new QLabel("点位输出", this);
    outputLabel->setFixedSize(100, 30);

    // 点位选择 1-16
    outputPointCombo = new QComboBox(this);
    for (int i = 1; i <= 16; ++i)
        outputPointCombo->addItem(QString::number(i));
    outputPointCombo->setFixedWidth(80);

    // 输出值 true / false
    outputValueCombo = new QComboBox(this);
    outputValueCombo->addItem("true");
    outputValueCombo->addItem("false");
    outputValueCombo->setFixedWidth(80);

    outputLayout->addWidget(outputLabel);
    outputLayout->addWidget(outputPointCombo);
    outputLayout->addWidget(outputValueCombo);
    outputLayout->addStretch();

    mainLayout->addLayout(outputLayout);

    // --- 信号连接 ---
    connect(fontSizeSlider, &QSlider::valueChanged,
        this, &DebugTool::onFontSizeSliderValueChanged);

    connect(disconnectCameraButton, &QPushButton::clicked,
        this, &DebugTool::onDisconnectCameraClicked);
}

void DebugTool::onDisconnectCameraClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "确认操作",
        "是否断开全部相机？",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        qDebug() << "用户确认断开全部相机";
        emit disconnectAllCameras();
        QMessageBox::information(this, "完成", "所有相机已断开。");
    }
    else {
        qDebug() << "用户取消断开相机操作";
    }
}

void DebugTool::onFontSizeSliderValueChanged(int value)
{
    if (fontSizeValueLabel) {
        fontSizeValueLabel->setText(QString::number(value));
    }
}

void DebugTool::SetIOLight()
{
    for (int ch = 1; ch <= 4; ++ch) {
        int value = 0;
        switch (ch) {
        case 1: value = GlobalPara::Light1; break;
        case 2: value = GlobalPara::Light2; break;
        case 3: value = GlobalPara::Light3; break;
        case 4: value = GlobalPara::Light4; break;
        }
        if (PCI::pci().setlight(ch, value, 100, 1, false, ch) != 0) {
            std::cerr << "[ERROR] " << ch << "通道光源设置失败" << std::endl;
        }
        else {
            std::cout << "[INFO] " << ch << "通道光源设置成功" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    int outPoint = outputPointCombo->currentText().toInt();
    bool outValue = (outputValueCombo->currentText() == "true");
    if(GlobalPara::ioType == GlobalPara::VC3000H){
        PCI::pci().setOutputMode(outPoint, outValue,300);
	}
    else if(GlobalPara::ioType == GlobalPara::VC5000)
    {
		VC5000DLL::vc5000_inst().setoutput(outPoint, outValue);
    }
}

void DebugTool::saveSettings()
{
    const QString settingsFilePath = SystemPara::OTHER_DIR;

    QMap<QString, int> paramMap;
    paramMap["FontSize"] = fontSizeSlider->value();
    paramMap["LIGHT1"] = light1Slider->value();
    paramMap["LIGHT2"] = light2Slider->value();
    paramMap["LIGHT3"] = light3Slider->value();
    paramMap["LIGHT4"] = light4Slider->value();
    paramMap["RunPoint"] = runPointCombo->currentText().toInt();
    paramMap["InputPoint"] = inputPointCombo->currentText().toInt();
    paramMap["LearnCount"] = learnCountEdit->text().toInt();
    paramMap["Timeout"] = timeoutEdit->text().toInt();

    // 更新全局变量
    GlobalPara::FontSize = paramMap["FontSize"];
    GlobalPara::Light1 = paramMap["LIGHT1"];
    GlobalPara::Light2 = paramMap["LIGHT2"];
    GlobalPara::Light3 = paramMap["LIGHT3"];
    GlobalPara::Light4 = paramMap["LIGHT4"];
    GlobalPara::RunPoint = paramMap["RunPoint"];
    GlobalPara::InputPoint = paramMap["InputPoint"];
    GlobalPara::LearnCount = paramMap["LearnCount"];
    GlobalPara::TimeOut = paramMap["Timeout"];

    SetIOLight();


    // 异步写入 JSON
    QtConcurrent::run([settingsFilePath, paramMap]() {
        QJsonObject rootObj = FileOperator::readJsonObject(settingsFilePath);
        if (rootObj.isEmpty()) {
            qWarning() << "Async Save Failed: Could not read valid JSON from" << settingsFilePath;
            return;
        }

        for (auto it = paramMap.constBegin(); it != paramMap.constEnd(); ++it) {
            const QString& key = it.key();
            int value = it.value();
            if (rootObj.contains(key) && rootObj[key].isObject()) {
                QJsonObject obj = rootObj[key].toObject();
                obj["值"] = value;
                rootObj[key] = obj;
                qDebug() << "Saved" << key << ":" << value;
            }
            else {
                qWarning() << "Key" << key << "not found in JSON.";
            }
        }

        if (!FileOperator::writeJsonObject(settingsFilePath, rootObj)) {
            qCritical() << "Async Save Failed: Could not write JSON to" << settingsFilePath;
        }
        });

    qDebug() << "Settings save task submitted via QtConcurrent.";
}

void DebugTool::closeEvent(QCloseEvent* event)
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "保存确认",
        "是否在关闭前保存当前设置？",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
        QMessageBox::Yes
    );

    if (reply == QMessageBox::Yes) {
        saveSettings();
        event->accept();
    }
    else if (reply == QMessageBox::No) {
        event->accept();
    }
    else {
        event->ignore();
    }
}
