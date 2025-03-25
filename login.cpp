#include "login.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QApplication>
#include <QTimer>
#include <QComboBox>
#include <QMessageBox>
Login::Login(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("登录");
    setFixedSize(400, 250);

    int labelWidth = 100;  // 统一角色和密码标签的宽度
    int inputWidth = 250;  // 统一输入框的宽度

    virtualKeyboard = new VirtualBox();
    virtualKeyboard->hide();


    // 角色选择框
    QLabel *roleLabel = new QLabel("选择角色", this);
    roleLabel->setFixedWidth(labelWidth);
    roleLabel->setStyleSheet("font-size: 16px;");

    roleComboBox = new QComboBox(this);
    roleComboBox->setFixedSize(inputWidth, 30);
    roleComboBox->setStyleSheet("font-size: 16px;");
    roleComboBox->addItems({"厂商", "机修", "操作员"});
    roleComboBox->setEditable(false);

    QHBoxLayout *roleLayout = new QHBoxLayout();
    roleLayout->addWidget(roleLabel);
    roleLayout->addWidget(roleComboBox);
    roleLayout->addStretch();  // 确保左对齐

    // 密码输入框
    QLabel *passwordLabel = new QLabel("输入密码", this);
    passwordLabel->setFixedWidth(labelWidth);
    passwordLabel->setStyleSheet("font-size: 16px;");

    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFixedSize(inputWidth, 40);
    passwordEdit->installEventFilter(this);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(passwordLabel);
    inputLayout->addWidget(passwordEdit);
    inputLayout->addStretch();  // 确保左对齐

    // 确认和取消按钮
    QPushButton *okButton = new QPushButton("确定", this);
    QPushButton *cancelButton = new QPushButton("取消", this);
    okButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    cancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(roleLayout);
    mainLayout->addLayout(inputLayout);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    qDebug() << "Connecting virtualKeyboard signal to handleKeyPress";
    qDebug() << "virtualKeyboard pointer:" << virtualKeyboard;


    connect(virtualKeyboard, &VirtualBox::keyPressed, this, &Login::handleKeyPress);
    connect(okButton, &QPushButton::clicked, this, &Login::onOkButtonClicked);
    connect(cancelButton, &QPushButton::clicked, this, [this]() {
        qDebug() << "取消按钮点击，关闭虚拟键盘";
        virtualKeyboard->hide();
        reject();
    });
}

// 登录验证接口
bool Login::validateLogin(const QString &role, const QString &password)
{
    // 示例验证：验证角色和密码是否正确
    // 可以根据实际需要调整验证规则，例如查询数据库或调用API
    if (role.isEmpty() || password.isEmpty()) {
        qDebug() << "角色或密码为空！";
        return false;
    }

    // 假设的角色和密码验证
    if (role == "厂商" && password == "password1") {
        return true;
    }
    if (role == "机修" && password == "password2") {
        return true;
    }
    if (role == "操作员" && password == "password3") {
        return true;
    }

    // 验证失败
    qDebug() << "验证失败，角色或密码错误！";
    return false;
}

// 确认按钮点击事件处理
void Login::onOkButtonClicked()
{
    qDebug() << "确认按钮点击，关闭虚拟键盘";

    QString selectedRole = roleComboBox->currentText();  // 获取选择的角色
    QString password = passwordEdit->text();  // 获取输入的密码

    // 调用登录验证函数
    if (validateLogin(selectedRole, password)) {
        virtualKeyboard->hide();  // 隐藏虚拟键盘
        accept();  // 登录成功，接受对话框
    } else {
        // 验证失败，提示错误信息
        QMessageBox::warning(this, "登录失败", "角色或密码错误，请重新输入！");
    }
}

bool Login::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == passwordEdit && event->type() == QEvent::MouseButtonPress) {
        qDebug() << "密码输入框被点击";
        passwordEdit->setFocus();
        showVirtualKeyboard();  // 显示虚拟键盘
        return true;
    }
    return QDialog::eventFilter(obj, event);
}

void Login::showVirtualKeyboard()
{
    QPoint dialogPos = this->mapToGlobal(QPoint(0, 0));
    int dialogBottom = dialogPos.y() + this->height();

    int x = dialogPos.x();
    int y = dialogBottom + 5;

    virtualKeyboard->move(x, y);
    virtualKeyboard->show();
    virtualKeyboard->raise();  // 提升到最前面

    // 延迟激活虚拟键盘
    QTimer::singleShot(0, this, [this]() {
        virtualKeyboard->activateWindow();
        virtualKeyboard->setFocus();  // 确保虚拟键盘获取焦点
    });
}


void Login::handleKeyPress(const QString &key)
{
     qDebug() << "handleKeyPress called with key:" << key;
    passwordEdit->setText(passwordEdit->text() + key);

}

QString Login::GetPassword() const {
    return passwordEdit->text();
}
