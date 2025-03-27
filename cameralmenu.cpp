#include "cameralmenu.h"

CameraMenu::CameraMenu(QWidget *parent)
    : QWidget(parent)
{
    // 创建菜单按钮
    menuButton = new QPushButton(this);
    menuButton->setText("▼"); // 使用向下箭头作为按钮文本
    menuButton->setFixedSize(20, 20); // 设置按钮大小
    menuButton->setStyleSheet("background-color: white; border: 1px solid gray;");

    // 创建菜单
    menu = new QMenu(this);

    // 连接按钮点击信号到菜单显示
    connect(menuButton, &QPushButton::clicked, [this]() {
        menu->exec(menuButton->mapToGlobal(QPoint(0, menuButton->height())));
    });
}

CameraMenu::~CameraMenu()
{
    delete menuButton; // 菜单按钮
    delete menu;
}

void CameraMenu::addMenuOption(const QString &optionName, const std::function<void()> &callback)
{
    QAction *action = menu->addAction(optionName);
    connect(action, &QAction::triggered, this, [callback]() {
        callback(); // 执行回调函数
    });
}
