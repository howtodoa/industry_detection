#include "virtualbox.h"
#include <qdebug.h>

VirtualBox::VirtualBox(QWidget *parent) : QWidget(parent) {
    mainLayout = new QGridLayout(this);
    createKeyboard();
    setLayout(mainLayout);
}

VirtualBox::~VirtualBox() {}

void VirtualBox::createKeyboard() {
    const QString keys[] = {
        "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
        "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
        "A", "S", "D", "F", "G", "H", "J", "K", "L",
        "Z", "X", "C", "V", "B", "N", "M"
    };

    int row = 0, col = 0;

    // 第一行：数字 0-9
    for (int i = 0; i < 10; i++, col++) {
        QPushButton *button = new QPushButton(keys[i]);
        button->setFixedSize(50, 50);
        mainLayout->addWidget(button, row, col);
        connect(button, &QPushButton::clicked, this, [this, i]() { onKeyClicked(i); });
    }

    // 第二行：Q-P
    row++; col = 0;
    for (int i = 10; i < 20; i++, col++) {
        QPushButton *button = new QPushButton(keys[i]);
        button->setFixedSize(50, 50);
        mainLayout->addWidget(button, row, col);
        connect(button, &QPushButton::clicked, this, [this, i]() { onKeyClicked(i); });
    }

    // 第三行：A-L（向右偏移 1）
    row++; col = 1;
    for (int i = 20; i < 29; i++, col++) {
        QPushButton *button = new QPushButton(keys[i]);
        button->setFixedSize(50, 50);
        mainLayout->addWidget(button, row, col);

        bool success = connect(button, &QPushButton::clicked, this, [this, i]() {
            qDebug() << "按钮点击，i = " << i;
            onKeyClicked(i);
            qDebug() << "调用 onKeyClicked() 后";  // 确保函数被调用
        });
        if (!success) {
            qDebug() << "连接失败: button clicked -> onKeyClicked";
        } else {
            qDebug() << "连接成功1: button clicked -> onKeyClicked";
        }

    }

    // 第四行：Z-M（向右偏移 2）
    row++; col = 2;
    for (int i = 29; i < 36; i++, col++) {
        QPushButton *button = new QPushButton(keys[i]);
        button->setFixedSize(50, 50);
        mainLayout->addWidget(button, row, col);
        connect(button, &QPushButton::clicked, this, [this, i]() { onKeyClicked(i); });
    }
}

void VirtualBox::onKeyClicked(int keyId) {
    const QString keys[] = {
        "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
        "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
        "A", "S", "D", "F", "G", "H", "J", "K", "L",
        "Z", "X", "C", "V", "B", "N", "M"
    };
    qDebug()<<"aaa";
    if (keyId >= 0 && keyId < 36) {
        qDebug() << "按键输入：" << keys[keyId].toLower();

        emit keyPressed(keys[keyId].toLower());  // 统一转换为小写
    } else {
        qDebug() << "无效的按键ID：" << keyId;
    }
}
