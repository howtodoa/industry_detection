#include "DebugTool.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug> 
#include "typdef.h"
#include "fileoperator.h"
#include <QtConcurrent/QtConcurrent>
#include <QThreadPool>

DebugTool::DebugTool(QWidget* parent)
    : QWidget(parent)
{
    this->setWindowTitle("开发者工具");
    this->resize(400, 300);

    // 整体竖向布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // === 字体大小（示例布局结构，现在使用成员变量） ===
    QHBoxLayout* fontSizeLayout = new QHBoxLayout;
    fontSizeLayout->setSpacing(5);

    QPushButton* fontSizeButton = new QPushButton("字体大小", this);
    fontSizeButton->setFixedSize(160, 30);

    fontSizeSlider = new QSlider(Qt::Horizontal, this);
    fontSizeSlider->setRange(10, 30);
    fontSizeSlider->setValue(GlobalPara::FontSize);    // 默认值


    fontSizeValueLabel = new QLabel(QString::number(fontSizeSlider->value()), this);
    fontSizeValueLabel->setFixedWidth(30);
    fontSizeValueLabel->setAlignment(Qt::AlignCenter);

    fontSizeLayout->addWidget(fontSizeButton);
    fontSizeLayout->addWidget(fontSizeSlider);
    fontSizeLayout->addWidget(fontSizeValueLabel);
    fontSizeLayout->addStretch();

    mainLayout->addLayout(fontSizeLayout);

    mainLayout->addStretch();

    connect(fontSizeSlider, &QSlider::valueChanged,
        this, &DebugTool::onFontSizeSliderValueChanged);

    setLayout(mainLayout);
}

// 槽函数实现：实时更新字体大小显示值
void DebugTool::onFontSizeSliderValueChanged(int value)
{
    if (fontSizeValueLabel) {
        fontSizeValueLabel->setText(QString::number(value));
    }
}

void DebugTool::saveSettings()
{
    // 1. 获取要保存的值
    int currentFontSize = fontSizeSlider->value();

	GlobalPara::FontSize = currentFontSize; // 更新全局变量

    // 2. 定义配置文件路径和目标键
    const QString settingsFilePath = SystemPara::GLOBAL_DIR; 
    const QString targetKey = "FontSize"; 

    // 捕获列表
    QtConcurrent::run([settingsFilePath, targetKey, currentFontSize]() {

        // --- 1. 读取 JSON 文件内容 (使用 FileOperator) ---
        QJsonObject rootObj = FileOperator::readJsonObject(settingsFilePath);

        if (rootObj.isEmpty()) {
            qWarning() << "Async Save Failed: Could not read valid JSON from" << settingsFilePath;
            return;
        }

        // --- 2. 找到对应的字段并修改 ---
        if (rootObj.contains(targetKey) && rootObj[targetKey].isObject()) {

            QJsonObject settingObj = rootObj[targetKey].toObject();

            // 确保只修改 "值" 字段，保留其他字段
            settingObj["值"] = currentFontSize;

            rootObj[targetKey] = settingObj;

            if (FileOperator::writeJsonObject(settingsFilePath, rootObj)) {
                qDebug() << "Async Save Successful. Modified" << targetKey << "to" << currentFontSize;
            }
            else {
                qCritical() << "Async Save Failed: Could not write JSON to" << settingsFilePath;
            }
        }
        else {
            qWarning() << "Async Save Warning: Target key '" << targetKey << "' not found or invalid in JSON.";
        }
        }); // Lambda 表达式结束

    qDebug() << "Settings save task submitted via QtConcurrent.";
}


// closeEvent 保持不变
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
        saveSettings();  // 调用保存逻辑
        event->accept(); // 继续关闭
    }
    else if (reply == QMessageBox::No) {
        event->accept(); // 不保存也关闭
    }
    else {
        event->ignore(); // Cancel，不关闭
    }
}