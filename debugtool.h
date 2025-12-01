#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
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
    QSlider* fontSizeSlider = nullptr;
    QLabel* fontSizeValueLabel = nullptr;

    QSlider* light1Slider = nullptr;
    QSlider* light2Slider = nullptr;
    QSlider* light3Slider = nullptr;
    QSlider* light4Slider = nullptr;

    QComboBox* runPointCombo = nullptr;

    QComboBox* flowerPosCombo = nullptr;
    QComboBox* flowerNegCombo = nullptr;

    QPushButton* disconnectCameraButton = nullptr;
};
