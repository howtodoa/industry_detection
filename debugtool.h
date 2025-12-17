#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QCloseEvent>

class DebugTool : public QWidget
{
    Q_OBJECT
public:
    explicit DebugTool(QWidget* parent = nullptr);
    void SetLight();
    void saveSettings();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onFontSizeSliderValueChanged(int value);
    void onDisconnectCameraClicked();

signals:
    void disconnectAllCameras();

private:
    // 字体大小
    QSlider* fontSizeSlider = nullptr;
    QLabel* fontSizeValueLabel = nullptr;

    // 光源滑块
    QSlider* light1Slider = nullptr;
    QSlider* light2Slider = nullptr;
    QSlider* light3Slider = nullptr;
    QSlider* light4Slider = nullptr;

    // 运行点位下拉框
    QComboBox* runPointCombo = nullptr;

    // Flower 长度下拉框
    QComboBox* flowerPosCombo = nullptr;
    QComboBox* flowerNegCombo = nullptr;

    // 输入点位（和运行点位一样的下拉框 1-8）
    QComboBox* inputPointCombo = nullptr;

    // 断开相机按钮
    QPushButton* disconnectCameraButton = nullptr;

    // 自学习次数
    QPushButton* learnCountButton = nullptr;
    QLineEdit* learnCountEdit = nullptr;

    // 超时时间
    QPushButton* timeoutButton = nullptr;
    QLineEdit* timeoutEdit = nullptr;
};
