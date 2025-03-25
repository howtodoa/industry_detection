#ifndef VIRTUALBOX_H
#define VIRTUALBOX_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QDebug>

class VirtualBox : public QWidget
{
    Q_OBJECT
public:
    explicit VirtualBox(QWidget *parent = nullptr);
    ~VirtualBox();

signals:
    void keyPressed(const QString &key);  // 发送按键信号

private slots:
    void onKeyClicked(int keyId);  // 处理按键点击

private:
    void createKeyboard();  // 创建虚拟键盘
    QGridLayout *mainLayout;
};
#endif // VIRTUALBOX_H
