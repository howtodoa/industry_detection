#ifndef DEBUGTOOL_H
#define DEBUGTOOL_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QCloseEvent>

class DebugTool : public QWidget
{
    Q_OBJECT

public:
    explicit DebugTool(QWidget* parent = nullptr);
    ~DebugTool() override = default;

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    // 处理滑动条值变化的槽函数（用于实时更新 QLabel 显示）
    void onFontSizeSliderValueChanged(int value);

private:
    // 供 closeEvent 调用的保存逻辑函数
    void saveSettings();

    // === 声明为类的私有成员变量，以便在 saveSettings() 中访问 ===
    QSlider* fontSizeSlider;
    QLabel* fontSizeValueLabel; // 更改命名以区分和明确它是 QLabel
    // 您可以根据需要添加其他控件的成员变量
};

#endif // DEBUGTOOL_H