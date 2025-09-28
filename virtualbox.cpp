#include "virtualbox.h"

VirtualBox::VirtualBox(QWidget *parent) : QWidget(parent) {
    mainLayout = new QGridLayout(this);
    createKeyboard();
    setLayout(mainLayout);
}

VirtualBox::~VirtualBox() {
    delete mainLayout;
}

void VirtualBox::createKeyboard() {
    const QString keys[] = {
        "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
        "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
        "A", "S", "D", "F", "G", "H", "J", "K", "L",
        "Z", "X", "C", "V", "B", "N", "M"
    };

    QFont buttonFont;
    buttonFont.setPointSize(18);
    buttonFont.setBold(true);

    int row = 0, col = 0;

    // 第一行：数字 0-9
    for (int i = 0; i < 10; i++, col++) {
        QPushButton *button = new QPushButton(keys[i]);
        button->setFont(buttonFont);
        button->setFixedSize(50, 50);
        mainLayout->addWidget(button, row, col);
        connect(button, &QPushButton::clicked, this, [this, i]() { onKeyClicked(i); });
    }

    // 第二行：Q - P
    row++; col = 0;
    for (int i = 10; i < 20; i++, col++) {
        QPushButton *button = new QPushButton(keys[i]);
        button->setFont(buttonFont);
        button->setFixedSize(50, 50);
        mainLayout->addWidget(button, row, col);
        keyButtons.append(button);  // 存储以便切换大小写
        connect(button, &QPushButton::clicked, this, [this, i]() { onKeyClicked(i); });
    }

    // 第三行：A - L，左移（col = 0）
    row++; col = 0;
    for (int i = 20; i < 29; i++, col++) {
        QPushButton *button = new QPushButton(keys[i]);
        button->setFont(buttonFont);
        button->setFixedSize(50, 50);
        mainLayout->addWidget(button, row, col);
        keyButtons.append(button);
        connect(button, &QPushButton::clicked, this, [this, i]() { onKeyClicked(i); });
    }

    // 第三行末尾添加 Shift 键
    QPushButton *shiftButton = new QPushButton("↑");
    shiftButton->setFont(buttonFont);
    shiftButton->setFixedSize(50, 50);
    mainLayout->addWidget(shiftButton, row, col++);
    connect(shiftButton, &QPushButton::clicked, this, &VirtualBox::onShiftClicked);

    // 第四行：Z - M，左移（col = 0）
    row++; col = 0;
    for (int i = 29; i < 36; i++, col++) {
        QPushButton *button = new QPushButton(keys[i]);
        button->setFont(buttonFont);
        button->setFixedSize(50, 50);
        mainLayout->addWidget(button, row, col);
        keyButtons.append(button);
        connect(button, &QPushButton::clicked, this, [this, i]() { onKeyClicked(i); });
    }

    // 删除键
    QPushButton *backspaceButton = new QPushButton("⌫");
    backspaceButton->setFont(buttonFont);
    backspaceButton->setFixedSize(50, 50);
    mainLayout->addWidget(backspaceButton, row, col++);
    connect(backspaceButton, &QPushButton::clicked, this, [this]() {
        onKeyClicked(-1);
    });

    // 关闭键盘按钮（Enter箭头）
    QPushButton *closeButton = new QPushButton("↵");
    closeButton->setFont(buttonFont);
    closeButton->setFixedSize(50, 50);
    mainLayout->addWidget(closeButton, row, col++);
    connect(closeButton, &QPushButton::clicked, this, [this]() {
        emit requestClose();
        this->hide();
    });
}

void VirtualBox::onKeyClicked(int keyId) {
    const QString keys[] = {
        "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
        "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
        "A", "S", "D", "F", "G", "H", "J", "K", "L",
        "Z", "X", "C", "V", "B", "N", "M"
    };

    if (keyId == -1) {
        emit keyPressed("backspace");
    }
    else if (keyId >= 0 && keyId < 36) {
        QString key = keys[keyId];
        key = shiftOn ? key.toUpper() : key.toLower();
        emit keyPressed(key);
    }
    else {
        qDebug() << "无效的按键ID：" << keyId;
    }
}

void VirtualBox::onShiftClicked() {
    shiftOn = !shiftOn;
    updateKeyLabels();
}

void VirtualBox::updateKeyLabels() {
    const QString keys[] = {
        "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
        "A", "S", "D", "F", "G", "H", "J", "K", "L",
        "Z", "X", "C", "V", "B", "N", "M"
    };

    for (int i = 0; i < keyButtons.size(); ++i) {
        QString text = keys[i];
        keyButtons[i]->setText(shiftOn ? text.toUpper() : text.toLower());
    }
}
