#ifndef CAMERAMENU_H
#define CAMERAMENU_H

#include <QWidget>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QToolButton>

class CameraMenu : public QWidget {
    Q_OBJECT

public:
    explicit CameraMenu(QWidget *parent = nullptr);
    ~CameraMenu();
    // 获取菜单按钮
    QPushButton* getMenuButton() const { return menuButton; }

    // 添加菜单选项
    void addMenuOption(const QString &optionName, const std::function<void()> &callback);

private:
    QPushButton *menuButton; // 菜单按钮
    QMenu *menu;             // 菜单
};

#endif // CAMERAMENU_H
