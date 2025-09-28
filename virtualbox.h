#ifndef VIRTUALBOX_H
#define VIRTUALBOX_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QDebug>
#include <QVector>

class VirtualBox : public QWidget
{
    Q_OBJECT
public:
    explicit VirtualBox(QWidget *parent = nullptr);
    ~VirtualBox();

signals:
    void keyPressed(const QString &key);  // 发送按键信号
    void requestClose();                 // 请求关闭键盘

private slots:
    void onKeyClicked(int keyId);        // 普通按键点击
    void onShiftClicked();               // Shift 键点击

private:
    void createKeyboard();               // 创建虚拟键盘
    void updateKeyLabels();              // 切换大小写时更新按钮文本

    QGridLayout *mainLayout;
    bool shiftOn = false;                // 大小写状态
    QVector<QPushButton*> keyButtons;    // 存储所有字母按键，方便大小写切换
};

#endif // VIRTUALBOX_H
